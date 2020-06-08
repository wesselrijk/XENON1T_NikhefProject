"""I/O plugin base classes for input to/from folders or zipfiles
"""
import errno
import glob
import zlib
import os
import shutil
from operator import itemgetter
import logging
from time import strftime

from six.moves import input
import utils
from pax.datastructure import make_event_proxy
    
class InputFromFolder(object):
    """Read from a folder containing several small files, each containing events"""

    ##
    # Base class methods you don't need to override
    ##

    def __init__(self, dir_name):
        self.name = self.__class__.__name__
        self.log = logging.getLogger(self.name)
        self.total_time_taken = 0   # Total time in msec spent in this plugin
        self.dir_name = dir_name
        self._pre_startup()
        y = self.startup()
        if y is not None:
            raise RuntimeError('Startup of %s returned a %s instead of None.' % 
(self.name, type(y)))
        
    def _pre_startup(self):
        pass

    def startup(self):
        self.raw_data_files = []
        self.current_file_number = None
        self.current_filename = None
        self.current_first_event = None
        self.current_last_event = None
        self.current_event = None
        input_name = self.dir_name
#        input_name = utils.data_file_name(self.config['input_name'])
        if not os.path.exists(input_name):
            raise ValueError("Can't read from %s: it does not exist!" % input_name)

        if not os.path.isdir(input_name):
            if not input_name.endswith('.' + self.file_extension):
                self.log.error("input_name %s does not end "
                               "with the expected file extension %s" % (input_name,
                                                                        self.file_extension))
                return
            self.log.debug("InputFromFolder: Single file mode")
            self.init_file(input_name)

        else:
            self.log.debug("InputFromFolder: Directory mode")
            file_names = glob.glob(os.path.join(input_name, "*." + self.file_extension))
            # Remove the pax_info.json file from the file list-- the JSON I/O will thank us
            file_names = [fn for fn in file_names if not fn.endswith('pax_info.json')]
            file_names.sort()
            self.log.debug("InputFromFolder: Found these files: %s", str(file_names))
            if len(file_names) == 0:
                raise ValueError("InputFromFolder: No %s files found in input directory %s!" % (self.file_extension,
                                                                                                input_name))
            for fn in file_names:
                if 'trigger_monitor_data.' in fn:
                    continue
                if 'temp.' in fn:
                    self.log.warning("Temporary raw data file found in directory: this data is still being built "
                                     "or has crashed while building!")
                    continue
                self.init_file(fn)

        # Sort the files by first event number, so events are read in order
        # Files are read in lexically, but in some cases that may not reflect the event order (see issue #345)
        self.raw_data_files = sorted(self.raw_data_files, key=itemgetter('first_event'))

        # Select the first file
        self.select_file(0)

        # Set the number of total events
        self.number_of_events = sum([fr['n_events'] for fr in self.raw_data_files])

    def transform_event(self, event_proxy):
        """Uncompress the compressed event and return the pickle"""
        data = zlib.decompress(event_proxy.data)
        event = self.decode_event(data)
        event.block_id = event_proxy.block_id
        return event

    def decode_event(self, event_proxy):
        """The event is pickled"""
        return pickle.loads(event_proxy)

    def init_file(self, filename):
        """Find out the first and last event contained in filename
        Appends {'filename': ..., 'first_event': ..., 'last_event':..., 'n_events':...} to self.raw_data_files
        """
        first_event, last_event, n_events = self.get_event_number_info(filename)
        self.log.debug("InputFromFolder: Initializing %s", filename)
        self.raw_data_files.append({'filename': filename,
                                    'first_event': first_event,
                                    'last_event': last_event,
                                    'n_events': n_events})

    def select_file(self, i):
        """Selects the ith file from self.raw_data_files for reading
        Will be called by get_single_event (and once by startup)
        """
        if self.current_file_number is not None:
            self.close()
        if i < 0 or i >= len(self.raw_data_files):
            raise RuntimeError("Invalid file index %s: %s files loaded" % (i,
                                                                           len(self.raw_data_files)))

        self.current_file_number = i
        f_info = self.raw_data_files[i]
        self.current_filename = f_info['filename']
        self.current_first_event = f_info['first_event']
        self.current_last_event = f_info['last_event']

        self.log.info("InputFromFolder: Selecting file %s "
                      "(number %d/%d in folder) for reading" % (self.current_filename,
                                                                i + 1,
                                                                len(self.raw_data_files)))

        self.open(self.current_filename)
        self.event_numbers_in_current_file = self.get_event_numbers_in_current_file()

    def shutdown(self):
        # hasattr check is needed to prevent extra error if pax crashes before the plugin runs
        if hasattr(self, 'current_file'):
            self.close()

    def get_events(self):
        """Iterate through all events in the file / folder"""
        # We must keep track of time ourselves, BasePlugin.timeit is a function decorator,
        # so it won't work well with generators like get_events for an input plugin
        for file_i, file_info in enumerate(self.raw_data_files):
            if self.current_file_number != file_i:
                self.select_file(file_i)
            for ev_proxy in self.get_all_events_in_current_file():
                event = self.transform_event(ev_proxy)
                yield event

    def get_single_event(self, event_number):
        """Get a single event, automatically selecting the right file"""
        if not self.current_first_event <= event_number <= self.current_last_event:
            # Time to open a new file!
            self.log.debug("InputFromFolder: Event %d is not in the current file (%d-%d), "
                           "so opening a new file..." % (event_number,
                                                         self.current_first_event,
                                                         self.current_last_event))
            for i, file_info in enumerate(self.raw_data_files):
                if file_info['first_event'] <= event_number <= file_info['last_event']:
                    self.select_file(i)
                    break
            else:
                raise ValueError("None of the loaded files contains event %d! "
                                 "Available event ranges: %s" % (event_number, [(q['first_event'], q['last_event'])
                                                                                for q in self.raw_data_files]))

        if event_number not in self.event_numbers_in_current_file:
            raise ValueError("Event %d does not exist in the file containing events %d - %d!\n"
                             "Event numbers which do exist in file: %s" % (event_number,
                                                                           self.current_first_event,
                                                                           self.current_last_event,
                                                                           self.event_numbers_in_current_file))

        ev_proxy = self.get_single_event_in_current_file(event_number)
        event = self.transform_event(ev_proxy)
        return event

    # If reading in from a folder-of-files format not written by FolderIO,
    # you'll probably have to overwrite this. (e.g. ReadXED does)
    def get_event_number_info(self, filename):
        """Return the first, last and total event numbers in file specified by filename"""
        stuff = os.path.splitext(os.path.basename(filename))[0].split('-')
        if len(stuff) == 4:
            # Old format, which didn't have an event numbers field... progress bar will be off...
            _, _, first_event, last_event = stuff
            return int(first_event), int(last_event), int(last_event) - int(first_event) + 1
        elif len(stuff) == 5:
            _, _, first_event, last_event, n_events = stuff
            return int(first_event), int(last_event), int(n_events)
        else:
            raise ValueError("Invalid file name: %s. Should be tpcname-something-firstevent-lastevent-nevents.%s" % (
                filename, self.file_extension))

    ##
    # Child class should override these
    ##

    file_extension = ''

    def open(self, filename):
        """Opens the file specified by filename for reading"""
        raise NotImplementedError()

    def close(self):
        """Close the currently open file"""
        pass

    ##
    # Override this if you support non-continuous event numbers
    ##
    def get_event_numbers_in_current_file(self):
        return list(range(self.current_first_event, self.current_last_event + 1))

    ##
    # Override this if you support random access
    ##
    def get_single_event_in_current_file(self, event_number):
        """Uses iteration to emulate random access to events
        This does not check if the event actually exist: get_events is supposed to do that.
        """
        for event_i, event in enumerate(self.get_all_events_in_current_file()):
            if event.event_number == event_number:
                return event
        raise RuntimeError("Current file has no event %d, and some check didn't pick this up.\n"
                           "Either the file is very nasty, or the reader is bugged!" % event_number)

    ##
    # Override this if you DO NOT support random access, or if random access is slower than iteration
    ##
    def get_all_events_in_current_file(self):
        """Uses random access to iterate over all events"""
        for event_number in self.event_numbers_in_current_file:
            yield self.get_single_event_in_current_file(event_number)

import pickle
import io
class RestrictedUnPickle(pickle.Unpickler):
    def find_class(self, module, name):
        module.replace("pax.", "")
        print("Module: ", module, name)
        if module == "pax.datastructure":
            print("Skipping")
        return getattr(module, name)

    

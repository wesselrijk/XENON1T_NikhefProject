
#include "YourActionInitialization.hh"
#include "UserRootActions.hh"
#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"

YourActionInitialization::YourActionInitialization(YourDetectorConstruction* det) 
: G4VUserActionInitialization() {
  fYourDetector = det;  
}

YourActionInitialization::~YourActionInitialization() {}

void YourActionInitialization::Build() const {
  YourPrimaryGeneratorAction* primayGenerator = new YourPrimaryGeneratorAction( fYourDetector );
  SetUserAction( primayGenerator) ;
  YourEventAction *eventAction = new YourEventAction();
  SetUserAction(eventAction);
  YourRunAction *runAction = new YourRunAction(eventAction);
  SetUserAction(runAction);
  YourTrackingAction *trackAction = new YourTrackingAction();
  SetUserAction(trackAction);
  YourSteppingAction *stepAction = new YourSteppingAction(eventAction);
  SetUserAction(stepAction);
  
}
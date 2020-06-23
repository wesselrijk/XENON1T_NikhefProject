//
// This include file has all the user classes that will be called.
// I put them into one include file for brevity and to show you the code together
// with the class definition.
//
// Please note that this is NOT the right way to do it!
// 
// All classes should have _separate_ definition and implementation files to
// help the build system decide when to recompile (i.e. dependencies).

#ifndef __UserActions__
#define __UserActions__

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserSteppingAction.hh"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"

// This class will get called once every Run, i.e. /run/beamOn 10 will call it 1 time
class YourRunAction : public G4UserRunAction {
public:
  YourRunAction() { }
  virtual ~YourRunAction() { }
  
  virtual G4Run* GenerateRun()
  { G4cout << "YourRunAction::GenerateRun() called" << G4endl; return new G4Run(); }
  virtual void   BeginOfRunAction(const G4Run* run)
  { G4cout << "YourRunAction::BeginOfRunAction() called" << G4endl; }
  virtual void   EndOfRunAction(const G4Run* run)
  { G4cout << "YourRunAction::EndOfRunAction() called" << G4endl; }
};

// This class will be called for every event, i.e. /run/beamOn 10 will call it 10 times
class YourEventAction : public G4UserEventAction {
public:
  YourEventAction() { } 
  virtual ~YourEventAction() { }
  
  virtual void BeginOfEventAction(const G4Event* ev)
  { G4cout << "..YourEventAction::BeginOfEventAction() called" << G4endl; }
  virtual void EndOfEventAction(const G4Event* ev)
  { G4cout << "..YourEventAction::EndOfEventAction() called" << G4endl; }
};

// This class will get called for every track that gets made.
class YourTrackingAction : public G4UserTrackingAction {
public:
  YourTrackingAction() { } 
  virtual ~YourTrackingAction() { }
  
  virtual void PreUserTrackingAction(const G4Track* tr)
  { G4cout << "....YourTrackingAction::PreUserTrackingAction() called: "
	   << "particle type: " << tr->GetParticleDefinition()->GetParticleName()
	   << G4endl;
  }
  virtual void PostUserTrackingAction(const G4Track* tr)
  { G4cout << "....YourTrackingAction::PostUserTrackingAction() called" << G4endl; }
};

// This class gets called for every Step
class YourSteppingAction : public G4UserSteppingAction {
public:
  YourSteppingAction() { } 
  virtual ~YourSteppingAction() { }
  
  virtual void UserSteppingAction(const G4Step* step)
  { G4cout << "......YourSteppingAction::UserSteppingAction() called: "
	   << " dE= " << step->GetTotalEnergyDeposit() << " MeV"
	   << G4endl;
  }
};

#endif // __UserActions__

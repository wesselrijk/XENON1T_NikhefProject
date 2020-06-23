
#include "globals.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "G4ScoringManager.hh"
#include "G4EventManager.hh"

#include "G4PhysListFactory.hh"

#include "FTFP_BERT.hh" // physics list, double for now, since already obtained from the factory
#include "G4OpticalPhysics.hh" // for optical physics (includes optical photon?)
#include "G4EmStandardPhysics_option4.hh" // physics list

#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"
#include "YourActionInitialization.hh"

#include "UltraPhysicsList.hh" // use ultra physics list for optical stuff included?

int main(int argc, char** argv) {

//(lets use C++ implementation of Jame's RANLUX generator)
  G4Random::setTheEngine(new CLHEP::RanluxEngine);

  //1. create the RunManager object 
  G4RunManager* runManager = new G4RunManager();
  //G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
 
  //2. Create an object from YourDetectorConstruction and register in the Run-Manager
  YourDetectorConstruction* detector = new YourDetectorConstruction();
  runManager->SetUserInitialization( detector );
   
  //3. Create/obtain an Physics List and register it in the Run-Manager CHANGED
  G4PhysListFactory physListFactory;
  const G4String plName = "FTFP_BERT";
  G4VModularPhysicsList* physicsList = physListFactory.GetReferencePhysList( plName );

  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  opticalPhysics->SetWLSTimeProfile("delta");

  opticalPhysics->SetScintillationYieldFactor(1.0);
  opticalPhysics->SetScintillationExcitationRatio(1.0);

  opticalPhysics->SetMaxNumPhotonsPerStep(100);
  opticalPhysics->SetMaxBetaChangePerStep(10.0);
  
  opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);

  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);
  //runManager->SetUserInitialization(new UltraPhysicsList()); // Geant4 won't start

  //5. Create your action initialization and register in the Run-Manager
  runManager->SetUserInitialization( new YourActionInitialization( detector ) );
  
  //VisManager goes here:
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);

  // initialization of the run
  runManager->Initialize();
  
  // add verbosity
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(1);
  
  // simulate 10 events
  //runManager->BeamOn(10); 
  
  ui->SessionStart();

  // delete the Run-manager
  delete runManager;
  delete visManager;
  
  return 0;
}

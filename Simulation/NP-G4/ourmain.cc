
#include "globals.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "G4ScoringManager.hh"

#include "G4PhysListFactory.hh"

#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"
#include "YourActionInitialization.hh"

int main(int argc, char** argv) {
  
  //1. create the RunManager object 
  G4RunManager* runManager = new G4RunManager();
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
 
  //2. Create an object from YourDetectorConstruction and register in the Run-Manager
  YourDetectorConstruction* detector = new YourDetectorConstruction();
  runManager->SetUserInitialization( detector );
   
  //3. Create/obtain an Physics List and register it in the Run-Manager 
  G4PhysListFactory physListFactory;
  const G4String plName = "FTFP_BERT";
  G4VModularPhysicsList* pl = physListFactory.GetReferencePhysList( plName );
  runManager->SetUserInitialization( pl ); 

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
  runManager->BeamOn(10); 
  
  ui->SessionStart();

  // delete the Run-manager
  delete runManager; 
  
  return 0;
}

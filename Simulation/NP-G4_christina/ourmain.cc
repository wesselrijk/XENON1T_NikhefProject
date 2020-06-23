
#include "globals.hh"


#include "G4RunManager.hh"

#include "G4PhysListFactory.hh"


#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"
#include "YourActionInitialization.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

int main(int argc, char** argv) {

  //1. create the RunManager object 
  G4RunManager* runManager = new G4RunManager();
 
  //2. Create an object from YourDetectorConstruction and register in the Run-Manager
  YourDetectorConstruction* detector = new YourDetectorConstruction();
  runManager->SetUserInitialization( detector );
   
  //3. Create/obtain an Physics List and register it in the Run-Manager 
  G4PhysListFactory physListFactory;
  const G4String plName = "FTFP_BERT";
  G4VModularPhysicsList* pl = physListFactory.GetReferencePhysList( plName );
  runManager->SetUserInitialization( pl ); 
  //4. Create your action initialization and register in the Run-Manager
  runManager->SetUserInitialization( new YourActionInitialization( detector) );

  //5. VisManager goes here:
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  ui->SessionStart(); 

  // initialization of the run
  runManager->Initialize();

 

  return 0;



}

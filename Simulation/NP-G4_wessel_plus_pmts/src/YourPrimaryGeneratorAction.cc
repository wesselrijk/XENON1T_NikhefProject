
#include "globals.hh"

#include "YourPrimaryGeneratorAction.hh"
#include "YourDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4ParticleGun.hh"

#include "G4Electron.hh"

YourPrimaryGeneratorAction::YourPrimaryGeneratorAction(YourDetectorConstruction* det)
: G4VUserPrimaryGeneratorAction() {
  fYourDetector = det;
  // create particle gun and set default properties:
  fParticleGun  = new G4ParticleGun(1);
  // particle type: e-
  G4ParticleDefinition* part = G4Electron::Definition();
  fParticleGun->SetParticleDefinition(part);
  // direction: 1,0,0 i.e. to the target 
  fParticleGun->SetParticleMomentumDirection( G4ThreeVector(0.0, 0.0, 1.0) );
  // kinetic energy:  30 MeV
  fParticleGun->SetParticleEnergy(30.0*CLHEP::MeV);
  // set postion
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0) ); // G4ThreeVector(fYourDetector->GetGunXPosition()
}

YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction(){
  delete fParticleGun;
}

void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
  
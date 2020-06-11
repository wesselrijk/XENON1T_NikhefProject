
#ifndef YOURDETECTORCONSTRUCTION_HH
#define YOURDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4Material;

class YourDetectorConstruction : public G4VUserDetectorConstruction {
public: 
   YourDetectorConstruction();
  ~YourDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  
  const G4Material* GetTargetMaterial() { return fTargetMaterial; }

  const G4Material* GetSphereMaterial() { return fSphereMaterial; }
  
  G4double          GetTargetThickness() { return fTargetThickness; }
  
  G4double          GetGunXPosition() { return fGunXPosition; } 
  
private:
  
  G4Material* fTargetMaterial;   

  G4Material* fSphereMaterial;   
  
  G4double    fTargetThickness;
  
  G4double    fGunXPosition;
  
};

#endif

#ifndef YOURDETECTORCONSTRUCTION_HH
#define YOURDETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4Material;
class G4MaterialPropertiesTable;
class G4Tubs; 

class YourDetectorConstruction : public G4VUserDetectorConstruction {
public: 
   YourDetectorConstruction();
  ~YourDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  
  const G4Material* GetTargetMaterial() { return fTargetMaterial; }

  const G4Material* GetCylMaterial() { return fLXe; }
  
  G4double          GetTargetThickness() { return fTargetThickness; }
  
  G4double          GetGunXPosition() { return fGunXPosition; } 
  
private:

  void DefineMaterials();
  
  G4Material* fTargetMaterial;   
  
  G4Material* fLXe;
  
  G4MaterialPropertiesTable* fLXe_mt;
  
  G4double    fTargetThickness;
  
  G4double    fGunXPosition;
  
};

#endif

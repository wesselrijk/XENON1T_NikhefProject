
#ifndef YOURDETECTORCONSTRUCTION_HH
#define YOURDETECTORCONSTRUCTION_HH

class G4Material;
class G4MaterialPropertiesTable;
class G4Tubs;
class G4OpticalSurface;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Box;
class G4Tubs;
class G4Sphere;
class UltraDetectorMessenger;


#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4Cache.hh"

#include "globals.hh"

class YourDetectorConstruction : public G4VUserDetectorConstruction {
public: 
   YourDetectorConstruction();
  ~YourDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
  
  const G4Material* GetTargetMaterial() { return fTargetMaterial; }

  const G4Material* GetCylMaterial() { return fLXe; }
  
  G4double          GetTargetThickness() { return fTargetThickness; }
  
  G4double          GetGunXPosition() { return fGunXPosition; } 

  void SetReflectionType(G4String);
  
private:

  void DefineMaterials();
  
  G4Material* fTargetMaterial;  
  G4Material* Air; 
  G4Material* fLXe;
  G4Material* fGXe;
  G4Material* fPTFE;
  
  G4MaterialPropertiesTable* fLXe_mt;
  G4MaterialPropertiesTable* fGXe_mt;
  G4MaterialPropertiesTable* fPTFE_mt;
  
  G4double    fTargetThickness;
  G4double    fGunXPosition;

  G4OpticalSurface* fPTFEOpticalSurface;
  G4OpticalSurface* fReflectorOpticalSurface;

  G4double lambda_min ;
  G4double lambda_max ;

  UltraDetectorMessenger *fDetectorMessenger;
  G4String fReflectionType;
  
};

#endif

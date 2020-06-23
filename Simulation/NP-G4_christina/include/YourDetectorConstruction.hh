
#ifndef YOURDETECTORCONSTRUCTION_HH
#define YOURDETECTORCONSTRUCTION_HH

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "tls.hh"

#include "G4Material.hh"
#include "G4VisAttributes.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4Cache.hh"

class G4Material;
class G4MaterialPropertiesTable;
class G4PhysicalVolume;
class G4LogicalVolume;
class G4OpticalSurface;
class G4Tubs; 
class G4Box;
class G4Sphere;
class G4UserLimits;
class G4GlobalMagFieldMessenger;

class YourDetectorConstruction : public G4VUserDetectorConstruction {
public: 
   YourDetectorConstruction();
  ~YourDetectorConstruction();
  
  virtual G4VPhysicalVolume* Construct();
//virtual void ConstructSDandField();

  const G4Material* GetTargetMaterial() { return fTargetMaterial; }

  const G4Material* GetCylMaterial() { return fLXe; }
  
  G4double          GetTargetThickness() { return fTargetThickness; }
  
  G4double          GetGunXPosition() { return fGunXPosition; } 
  
//extras for the parametrisation of the pmt volumes from B2b example

  void SetPmtMaterial (G4String );
  void SetMaxStep (G4double );
  void SetCheckOverlaps(G4bool );

private:

  // methods
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();

  // data members
 
  G4LogicalVolume* fLogicTarget;
  G4LogicalVolume* fLogicPmt;

  G4Material* fTargetMaterial;   
  G4Material* fPmtMaterial;

  G4Material* fLXe;

  G4Material* fGXe;

  G4Material* fPTFE;

  G4Material* fQuartz;
  
  G4MaterialPropertiesTable* fLXe_mt;
  
  G4MaterialPropertiesTable* fGXe_mt;

  G4MaterialPropertiesTable* fPTFE_mt;

  G4MaterialPropertiesTable* fQuartz_mt;


  G4double    fTargetThickness;
  
  G4double    fGunXPosition;
   
  G4UserLimits* fStepLimit;

  G4OpticalSurface* fPTFEOpticalSurface;
  G4OpticalSurface* fReflectorOpticalSurface;

  G4double lambda_min ;
  G4double lambda_max ;
 
 //B2bDetectorMessenger* fMessenger;
 //static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;

 G4bool fCheckOverlaps; //option to activate chacking of volume overlaps
};

#endif

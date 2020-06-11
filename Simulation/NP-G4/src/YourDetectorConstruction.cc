
#include "YourDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh" // table needed for LXe!
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"

// Adding system of units (ns, cm, MeV, etc.)
#include "G4SystemOfUnits.hh"

YourDetectorConstruction::YourDetectorConstruction() 
: G4VUserDetectorConstruction() {
  // set default target material
  G4String matName = "G4_Si";
  fTargetMaterial  = G4NistManager::Instance()->FindOrBuildMaterial(matName);
  if (fTargetMaterial == nullptr) {
    G4cerr << "  ERROR YourDetectorConstruction() \n" 
           << "  Material with name " << matName << " was not found! \n"
           << G4endl;
  }
  
  // set default target thickness
  fTargetThickness = 1.0*CLHEP::cm;
  // initial gun-x position 
  fGunXPosition    = -20;
  
  // DefineMaterials is used for liquid Xenon
  DefineMaterials();
}

YourDetectorConstruction::~YourDetectorConstruction() {}

void YourDetectorConstruction::DefineMaterials(){
  
  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;
  
  //***Materials
  //Liquid Xenon
  fLXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
  
  //***Material properties tables

  G4double lxe_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
  const G4int lxenum = sizeof(lxe_Energy)/sizeof(G4double);

  G4double lxe_SCINT[] = { 0.1, 1.0, 0.1 };
  assert(sizeof(lxe_SCINT) == sizeof(lxe_Energy));
  G4double lxe_RIND[]  = { 1.59 , 1.57, 1.54 };
  assert(sizeof(lxe_RIND) == sizeof(lxe_Energy));
  G4double lxe_ABSL[]  = { 35.*cm, 35.*cm, 35.*cm};
  assert(sizeof(lxe_ABSL) == sizeof(lxe_Energy));
  fLXe_mt = new G4MaterialPropertiesTable();
  fLXe_mt->AddProperty("FASTCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
  fLXe_mt->AddProperty("SLOWCOMPONENT", lxe_Energy, lxe_SCINT, lxenum);
  fLXe_mt->AddProperty("RINDEX",        lxe_Energy, lxe_RIND,  lxenum);
  fLXe_mt->AddProperty("ABSLENGTH",     lxe_Energy, lxe_ABSL,  lxenum);
  fLXe_mt->AddConstProperty("SCINTILLATIONYIELD",12000./MeV);
  fLXe_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
  fLXe_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
  fLXe_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
  fLXe_mt->AddConstProperty("YIELDRATIO",1.0);
  fLXe->SetMaterialPropertiesTable(fLXe_mt);

  // Set the Birks Constant for the LXe scintillator

  fLXe->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
  
}

G4VPhysicalVolume* YourDetectorConstruction::Construct() {
  G4cout << " === YourDetectorConstruction::Construct() method === " << G4endl;
  // define dimensions
  G4double targetXSize  = fTargetThickness;
  G4double targetYZSize = 1.25*targetXSize;
  G4double worldXSize   = 200*targetXSize;
  G4double worldYZSize  = 200*targetYZSize;
  // compute gun-x position 
  fGunXPosition  = -0.5*(targetXSize+worldXSize);
  // 
  G4double zet      = 1.0;
  G4double amass    = 1.01*CLHEP::g/CLHEP::mole;
  G4double density  = CLHEP::universe_mean_density;
  G4double pressure = 3.0E-18*CLHEP::pascal;
  G4double tempture = 2.73*CLHEP::kelvin;
  G4Material* materialWorld = new G4Material("Galactic", zet, amass, density, 
                                             kStateGas, tempture, pressure); 
 
  // world 
  G4Box* worldSolid = new G4Box( "solid-World",   // name
                                 0.5*worldXSize,  // box half x-size
                                 0.5*worldYZSize, // box half y-size
                                 0.5*worldYZSize  // box half z-size
                                );   
                                


  G4LogicalVolume* worldLogical = new G4LogicalVolume( worldSolid,    // solid 
                                                       materialWorld, // material                             
                                                       "logic-World"  // name
                                                     );       

  G4VPhysicalVolume* worldPhysical = new G4PVPlacement( nullptr,                    // (no) rotation 
                                                        G4ThreeVector(0., 0., 0.),  // translation
                                                        worldLogical,               // logical volume
                                                        "World",                    // name
                                                        nullptr,                    // mother volume (!)
                                                        false,                      // don't care
                                                        0                           // cpy number
                                                      );

  // target 
  G4Box*   targetSolid = new G4Box( "s0.5*targetXSizeolid-Target",
                                    0.5*targetXSize, 
                                    0.5*targetYZSize, 
                                    0.5*targetYZSize   
                                   );

                                   
  G4LogicalVolume* targetLogical = new G4LogicalVolume(targetSolid, 
                                                       fTargetMaterial, 
                                                       "logic-Target");                                 
  G4VPhysicalVolume* targetPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 0.),
                                                        targetLogical, 
                                                        "Target",
                                                        worldLogical,
                                                        false,
                                                        0);
  G4Tubs* cylSolid = new G4Tubs ( "solid-cylinder", 0, 96/2*targetXSize, 97/2*targetXSize,  0, 2*pi );           
                                  //12,20,30,0, 1.5*pi);


  G4LogicalVolume* cylLogical = new G4LogicalVolume(cylSolid, 
                                                       fLXe, 
                                                       "logic-cylinder");                                 
  G4VPhysicalVolume* cylPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 0.),
                                                        cylLogical, 
                                                        "Cylinder",
                                                        worldLogical,
                                                        false,
                                                        0);
   // return the root (i.e. world worldPhysical volume ptr)
   return worldPhysical;                                                         
}

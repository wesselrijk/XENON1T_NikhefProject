
#include "YourDetectorConstruction.hh"


#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4GeometryManager.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"

#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4Log.hh"

#include "G4SDManager.hh"


/*for reading in the PMT position data */
#include "input.hh"

// ------------------------------Start with detector construction---------------------------------  


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

  // DefineMaterials is used for liquid Xenon and PTFE materials
  DefineMaterials();

}


YourDetectorConstruction::~YourDetectorConstruction() {}

void YourDetectorConstruction::DefineMaterials(){

  // Define wavelength limits for materials definition
  lambda_min = 200*nm ; // 1.7 eV
  lambda_max = 700*nm ; // 6.1 eV

  // Nist manager
  G4NistManager* nistManager = G4NistManager::Instance();
  

  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;

  //***Elements
  G4Element* fC = new G4Element("C", "C", z=6., a=12.01*g/mole);
  G4Element* fF = new G4Element("F", "F", z=9, a=19.00*g/mole);
  G4Element* fO  = new G4Element("Oxygen", "O", z=8., a=16.00*g/mole);
  G4Element* fSi = new G4Element("Silicon", "Si", z=14., a=28.09*g/mole);
  
  //***Materials
  //Liquid Xenon
  fLXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
  // Xenon gas defined using NIST Manager
  fGXe = nistManager->FindOrBuildMaterial("G4_Xe");
  //PTFE
  fPTFE = new G4Material("PTFE", density=2.2*g/cm3, 2);
  fPTFE->AddElement(fC, 2);
  fPTFE->AddElement(fF, 4);
  // Quartz
  //  density = 2.200*g/cm3; // fused quartz 
  density = 2.64*g/cm3;  // crystalline quartz (c.f. PDG) 
  fQuartz = new G4Material("Quartz",density, 2);
  fQuartz->AddElement(fSi, 1) ;
  fQuartz->AddElement(fO , 2) ;

  
  //***Material properties tables
  //-------------------- Liquid Xenon Properties --------------------------------
  G4double lxe_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
  const G4int lxenum = sizeof(lxe_Energy)/sizeof(G4double);

  G4double lxe_SCINT[] = { 0.1, 1.0, 0.1 };
  assert(sizeof(lxe_SCINT) == sizeof(lxe_Energy));
  G4double lxe_RIND[]  = { 1.59 , 1.57, 1.54 };
  assert(sizeof(lxe_RIND) == sizeof(lxe_Energy));
  G4double lxe_ABSL[]  = { 50.*cm, 50.*cm, 50.*cm};
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

//------------------------ Teflon PTFE Properties -----------------------------------
  //fPTFE https://engineering.case.edu/centers/sdle/sites/engineering.case.edu.c
//enters.sdle/files/optical_properties_of_materials_for_concentrator_p.pdf
  // and also the https://arxiv.org/pdf/1512.07501.pdf article , this gives a re
//flectivity of about 93% (see refractive index wiki)
  fPTFE_mt = new G4MaterialPropertiesTable();
  const G4int NUM = 2;
  G4double XX[NUM] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} 
; 
  G4double PTFE_refl[NUM]      = { 0.95, 0.95 };
  G4double rIndexPTFE[]={ 1.38, 1.38};
  fPTFE_mt->AddProperty("RINDEX", XX,rIndexPTFE,NUM);
  fPTFE_mt->AddProperty("REFLECTIVITY", XX, PTFE_refl, NUM);
  fPTFE->SetMaterialPropertiesTable(fPTFE_mt);

  // Set the Birks Constant for the PTFE scintillator
  fPTFE->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

//-------------------------- Gas Xenon Proeperties ---------------------------------
  // fGXe properties table https://arxiv.org/pdf/1512.07501.pdf for refractive index
  fGXe_mt = new G4MaterialPropertiesTable();

  G4double wls_Energy2[] = {2.00*eV,2.87*eV,2.90*eV,3.47*eV};
  const G4int wlsnum2 = sizeof(wls_Energy2)/sizeof(G4double);
 
  G4double rIndexGXe[]={ 1., 1., 1., 1.};
  assert(sizeof(rIndexGXe) == sizeof(wls_Energy2));
  fGXe_mt->AddProperty("RINDEX", wls_Energy2,rIndexGXe,wlsnum2);
  fGXe->SetMaterialPropertiesTable(fGXe_mt);

  // Set the Birks Constant for the Gas Xenon 
  fGXe->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

//----------------------------- Quarts Properties -------------------------------------
//           Photomultiplier (PMT) window       
// The refractive index is for lime glass; 
// wavelength dependence is not included and value at 400nm is used.
  
  fQuartz_mt = new G4MaterialPropertiesTable();

  const G4int N_RINDEX_QUARTZ = 2 ;
  G4double X_RINDEX_QUARTZ[N_RINDEX_QUARTZ] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ; 
  G4double RINDEX_QUARTZ[N_RINDEX_QUARTZ] = {1.54, 1.54};

  
  fQuartz_mt->AddProperty("RINDEX", X_RINDEX_QUARTZ, RINDEX_QUARTZ, N_RINDEX_QUARTZ);
  fQuartz->SetMaterialPropertiesTable(fQuartz_mt);




}

 
// --------------------------- GEOMETRY -----------------------------

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
//----------------------------- WORLD -------------------------------

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

  // target world 
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

// ------------------------ Xenon Cylinder ------------------------------

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

//-------------------------- PTFE cylinder---------------------------------

  G4Tubs* cylPTFE = new G4Tubs ( "PTFE-cylinder", 96/2*targetXSize, 96/2*targetXSize + 5*targetXSize, 97/2*targetXSize,  0, 2*pi );
  G4LogicalVolume* logicalPTFE = new G4LogicalVolume(cylPTFE, 
                                                       fPTFE, 
                                                       "logic-TPC PTFE");                                 
  G4VPhysicalVolume* physicalPTFE = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 0.),
                                                        logicalPTFE, 
                                                        "Cylinder PTFE",
                                                        worldLogical,
                                                        false,
                                                        0);

//---------------------------Top Cap--------------------------------------

 G4Tubs* TopSolid = new G4Tubs ( "Top-cylinder", 0, 96/2*targetXSize, targetXSize,  0, 2*pi );


  G4LogicalVolume* TopLogical = new G4LogicalVolume(TopSolid, 
                                                       fPTFE,     //material is teflon
                                                       "logic-top");
  G4VPhysicalVolume* TopPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., 98/2*targetXSize),
                                                        TopLogical, 
                                                        "Top",
                                                        worldLogical, //mother volume is the world
                                                        false,
                                                        0);

//---------------------------Bottom Cap-----------------------------------

 G4Tubs* BottomSolid = new G4Tubs ( "Bottom-cylinder", 0, 96/2*targetXSize, targetXSize,  0, 2*pi );


  G4LogicalVolume* BottomLogical = new G4LogicalVolume(BottomSolid, 
                                                       fPTFE,       //material is teflon
                                                       "logic-bottom");                                 
  G4VPhysicalVolume* BottomPhysical = new G4PVPlacement(nullptr,
                                                        G4ThreeVector(0., 0., -98/2*targetXSize),
                                                        BottomLogical, 
                                                        "Bottom",
                                                        worldLogical, //mother volume is the world
                                                        false,
                                                        0);
//-------------------------------PMT--------------------------------------- 

//---------------------INPUT the coordinates of the PMTs from a .txt file----------------

  G4cout << "PMT nr. " << pmt_num[0] << G4endl;
  G4cout << "x[0] = " << x_pos[0] << G4endl;
  G4cout << "y[0] = " << y_pos[0] << G4endl;

 

//-------------------- Define the geometry of the PMTs ----------------------

G4Tubs* pmtSolid = new G4Tubs ( "pmt-cylinder", 0, 0.76*targetXSize, targetXSize,  0, 2*pi );           
                                  


  G4LogicalVolume* pmtLogical = new G4LogicalVolume(pmtSolid, 
                                                       fQuartz, 
                                                       "logic-pmt");
  for (int i=0; i<127; i++) {
      G4VPhysicalVolume* pmtPhysical = new G4PVPlacement(nullptr,
                                                            G4ThreeVector(x_pos[i]*targetXSize, y_pos[i]*targetXSize, 0.),
                                                            pmtLogical, 
                                                            "PMT",
                                                            TopLogical,
                                                            false,
                                                            0);

  }

  for (int j=127; j<248; j++) {
      G4VPhysicalVolume* pmtPhysical = new G4PVPlacement(nullptr,
                                                            G4ThreeVector(x_pos[j]*targetXSize, y_pos[j]*targetXSize, 0.),
                                                            pmtLogical, 
                                                            "PMT",
                                                            BottomLogical,
                                                            false,
                                                            0);
  }                                                          


   // return the root (i.e. world worldPhysical volume ptr)
   return worldPhysical;                                                         
}








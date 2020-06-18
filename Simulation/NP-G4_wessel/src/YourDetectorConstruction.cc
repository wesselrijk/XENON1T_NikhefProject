
#include "YourDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh" // table needed for LXe and PTFE!
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4Colour.hh" // for colour
#include "G4VisAttributes.hh" // for vis attributes
#include "G4OpticalSurface.hh" // for optical surface things
#include "G4OpBoundaryProcess.hh" // for optical boundary processes
#include "G4LogicalBorderSurface.hh" // for making bordersurfaces
#include "G4RunManager.hh" // for updating runmanager
#include "G4SystemOfUnits.hh"// Adding system of units (ns, cm, MeV, etc.)


#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4GeometryManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Log.hh"
#include "G4SDManager.hh"

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

  //***Materials
  //Liquid Xenon
  fLXe = new G4Material("LXe",z=54.,a=131.29*g/mole,density=3.020*g/cm3);
  // Xenon gas defined using NIST Manager
  fGXe = nistManager->FindOrBuildMaterial("G4_Xe");
  //PTFE
  fPTFE = new G4Material("PTFE", density=2.2*g/cm3, 2);
  fPTFE->AddElement(fC, 2);
  fPTFE->AddElement(fF, 4);
  
  //***Material properties tables
  // fLXe - example LXe and https://arxiv.org/pdf/1512.07501.pdf uses 1.63 for refractive index
  G4double lxe_Energy[]    = { 7.0*eV , 7.07*eV, 7.14*eV };
  const G4int lxenum = sizeof(lxe_Energy)/sizeof(G4double);

  G4double lxe_SCINT[] = { 0.1, 1.0, 0.1 };
  assert(sizeof(lxe_SCINT) == sizeof(lxe_Energy));
  G4double lxe_RIND[]  = { 1.59 , 1.57, 1.54 };
  assert(sizeof(lxe_RIND) == sizeof(lxe_Energy));
  G4double lxe_ABSL[]  = { 50.*m, 50.*m, 50.*m};
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

  //fPTFE https://engineering.case.edu/centers/sdle/sites/engineering.case.edu.centers.sdle/files/optical_properties_of_materials_for_concentrator_p.pdf
  // and also the https://arxiv.org/pdf/1512.07501.pdf article , this gives a reflectivity of about 93% (see refractive index wiki)
  fPTFE_mt = new G4MaterialPropertiesTable();
  const G4int NUM = 2;
  G4double XX[NUM] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ; 
  G4double PTFE_refl[NUM]      = { 0.95, 0.95 };
  G4double rIndexPTFE[]={ 1.38, 1.38};
  fPTFE_mt->AddProperty("RINDEX", XX,rIndexPTFE,NUM);
  fPTFE_mt->AddProperty("REFLECTIVITY", XX, PTFE_refl, NUM);
  fPTFE->SetMaterialPropertiesTable(fPTFE_mt);

  // Set the Birks Constant for the PTFE scintillator
  fPTFE->GetIonisation()->SetBirksConstant(0.126*mm/MeV);


  // fGXe properties table https://arxiv.org/pdf/1512.07501.pdf for refractive index
  fGXe_mt = new G4MaterialPropertiesTable();

  G4double wls_Energy2[] = {2.00*eV,2.87*eV,2.90*eV,3.47*eV};
  const G4int wlsnum2 = sizeof(wls_Energy2)/sizeof(G4double);
 
  G4double rIndexGXe[]={ 1., 1., 1., 1.};
  assert(sizeof(rIndexGXe) == sizeof(wls_Energy2));
  fGXe_mt->AddProperty("RINDEX", wls_Energy2,rIndexGXe,wlsnum2);
  fGXe->SetMaterialPropertiesTable(fGXe_mt);

  // Set the Birks Constant for the PTFE scintillator
  fGXe->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

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
  
  // LXe cylinder
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

  // PTFE cylinder
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

  // PTFE mirror
  const G4double x = 40.0*cm;
  const G4double y = 40.0*cm;
  const G4double z = 1*cm;
  G4Box * box = new G4Box("Mirror",x,y,z);
  G4LogicalVolume* fReflectorLog = new G4LogicalVolume(box,fPTFE,"Reflector",0,0,0);  
  G4ThreeVector SurfacePosition = G4ThreeVector(0.5*m,0*m,0) ;
  // Rotate reflecting surface by 45. degrees around the OX axis.
  G4RotationMatrix *Surfrot = new G4RotationMatrix(G4ThreeVector(0.0,1.0,0.0),-pi/4.);
  new G4PVPlacement(Surfrot,SurfacePosition,"MirrorPV",fReflectorLog,worldPhysical,false,0);                                             

  // Set visual attributes
  G4VisAttributes* XeVisAtt = new G4VisAttributes(G4Colour(0.1,0.9,0.1));
  XeVisAtt->SetVisibility(true);
  //XeVisAtt->SetForceWireframe(true);
  cylLogical->SetVisAttributes(XeVisAtt);

  G4VisAttributes* SurfaceVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  SurfaceVisAtt->SetVisibility(true);
  //SurfaceVisAtt->SetForceWireframe(true);
  logicalPTFE->SetVisAttributes(SurfaceVisAtt);

  // create optical surfaces
  fReflectorOpticalSurface = new G4OpticalSurface("ReflectorOpticalSurface");
  fReflectorOpticalSurface->SetModel(unified);
  fReflectorOpticalSurface->SetType(dielectric_dielectric);
  fReflectorOpticalSurface->SetFinish(polished);

  fPTFEOpticalSurface = new G4OpticalSurface("ReflectorOpticalSurface");
  fPTFEOpticalSurface->SetModel(unified);
  fPTFEOpticalSurface->SetType(dielectric_dielectric);
  fPTFEOpticalSurface->SetFinish(polished);

  // set bordersurface skinsurface and update runmanager
  //new G4LogicalBorderSurface("LXE PTFE surface", cylPhysical, physicalPTFE, fReflectorOpticalSurface);
  new G4LogicalSkinSurface("ReflectorSurface",fReflectorLog,fReflectorOpticalSurface);
  new G4LogicalSkinSurface("ReflectorSurface",logicalPTFE,fPTFEOpticalSurface);

  G4RunManager* runManager = G4RunManager::GetRunManager();
  runManager->GeometryHasBeenModified();

   // return the root (i.e. world worldPhysical volume ptr)
   return worldPhysical;                                                         
}

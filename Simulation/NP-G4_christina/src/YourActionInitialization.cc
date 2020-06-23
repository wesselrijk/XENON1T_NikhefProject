
#include "YourActionInitialization.hh"

#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"

YourActionInitialization::YourActionInitialization(YourDetectorConstruction* det) 
: G4VUserActionInitialization() {
  fYourDetector = det;  
}

YourActionInitialization::~YourActionInitialization() {}

void YourActionInitialization::Build() const {
  YourPrimaryGeneratorAction* primayGenerator = new YourPrimaryGeneratorAction( fYourDetector );
  SetUserAction( primayGenerator) ;
}
#include "MySD.hh"
#include "MyHit.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"


#include <fstream>
#include <iostream>
using namespace std;

// -------------------------------------------------------------------------
//  The sensitive detector class.
//  The function that's interesting is ProcessHits
//
// -------------------------------------------------------------------------
MySD::MySD(const G4String& name,
const G4String& hitsCollectionName) :
G4VSensitiveDetector(name), fHitsCollection(0)
{
collectionName.insert(hitsCollectionName);
}
MySD::~MySD()
{
}
void MySD::Initialize(G4HCofThisEvent* hce)
{
fHitsCollection 
= new MyHitsCollection(SensitiveDetectorName, collectionName[0]); 
auto hcID 
= G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
hce->AddHitsCollection( hcID, fHitsCollection ); 
}


G4bool MySD::ProcessHits(G4Step* step, G4TouchableHistory* /*history*/)
{
fHitsCollection = new MyHitsCollection(SensitiveDetectorName, collectionName[0]); 
MyHit* newHit = new MyHit();



// I use the pre point to get the kinetic energy of the particle (it's still called edep, sorry)
G4StepPoint* prePoint = step->GetPreStepPoint();
if (prePoint->GetMass() != 0) {
    return false;
}
G4double edep = prePoint->GetKineticEnergy();
G4double rel_time = prePoint->GetGlobalTime();
G4ThreeVector postion = prePoint->GetPosition();
std::string pmt = prePoint->GetPhysicalVolume()->GetName();
//G4cout << "the edep is " << edep << G4endl;
// We can add properties to Hit here that we want to save
// newHit->SetXYZ();
newHit->SetEdep(edep);
newHit->SetTime(rel_time);
newHit->SetPos(postion);
newHit->SetPMT(pmt);
//newHit->Draw();
//G4cout << newHit->GetEdep() << G4endl;

// add the hit to our hitlist _hits
if (edep > 0){
    _hits.push_back(newHit);
}   

// check if everything worked
//G4cout << "it's doing something!" << G4endl;

fstream ofile("data.dat", ios::out);
int counter = 1;
for (auto iterator=_hits.begin(); iterator!=_hits.end(); ++iterator, ++counter) 
{
    ofile << counter << ";" <<(*iterator)->GetPMT()<< ";" <<(*iterator)->GetEdep() << ";" << (*iterator)->GetTime() << ";" << (*iterator)->GetPos() << "\n";
}
ofile.close();

//G4cout << "end?" << G4endl;
return true;
}

// we can perhaps use this later
void MySD::EndOfEvent(G4HCofThisEvent* hce) //
{
    G4cout
    << G4endl 
    << "-------->Hits Collection: in this event they are "  
    << " hits in the tracker chambers: " 
    << " I FUCKING LOVE GEANT4 IT'S THE BEST THING EVER LIKE DAAAAAAAAMN"
    << G4endl;
    //for ( std::size_t i=0; i<_hits.size(); ++i ) (*fHitsCollection)[i]->Print();
    
}
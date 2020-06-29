
#ifndef MySD_h
#define MySD_h 1

#include "G4VSensitiveDetector.hh"
#include "MyHit.hh"



class MySD : public G4VSensitiveDetector {
    public:
    MySD(const G4String& name,
    const G4String& hitsCollectionName);
    MyHitsCollection* fHitsCollection;   
    std::list<MyHit*> _hits;
    virtual ~MySD();
    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step,
    G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hce);
};

#endif
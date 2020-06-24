#ifndef MyHit_h
#define MyHit_h 1
#include <list>
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
class MyHit : public G4VHit
{
public:
    MyHit();
    // set/get methods; eg.
    void SetEdep (G4double edep);
    G4double GetEdep();// const;
    virtual void Draw();
    virtual void Print();
    


private:
    // some data members; eg.
    G4double fEdep; // energy deposit



};
typedef G4THitsCollection<MyHit> MyHitsCollection;

#endif

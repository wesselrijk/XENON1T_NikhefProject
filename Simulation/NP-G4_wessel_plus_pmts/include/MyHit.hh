#ifndef MyHit_h
#define MyHit_h 1
#include <list>
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"


class MyHit : public G4VHit
{
public:
    MyHit();
    // set/get methods; eg.
    void SetEdep (G4double edep);
    G4double GetEdep();// const;
    void SetPos(G4ThreeVector pos);
    G4ThreeVector GetPos();// const
    void SetTime (G4double time);
    G4double GetTime();// const;

    virtual void Draw();
    virtual void Print();
    


private:
    // some data members; eg.
    G4double fEdep; // energy deposit
    G4ThreeVector _pos;
    G4double _time;


};
typedef G4THitsCollection<MyHit> MyHitsCollection;

#endif

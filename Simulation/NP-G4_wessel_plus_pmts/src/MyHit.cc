#include "MyHit.hh"
#include "G4ios.hh"

MyHit::MyHit()
:   G4VHit(),
    fEdep(0.)  
{}
void MyHit::Print() // more or less replaced by Draw().
{
  G4cout
     << "Print" 
     << G4endl;
}

void MyHit::SetEdep(G4double edep)
    {fEdep = edep; }

G4double MyHit::GetEdep(){
    return fEdep;
    }
void MyHit::Draw()
{   
    G4cout << "We are drawing them things" << G4endl;
    G4cout << "the deposited energy = " << fEdep << G4endl;
    G4cout << "Done drawing them things" << G4endl;
}

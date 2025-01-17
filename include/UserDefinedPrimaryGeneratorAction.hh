
#ifndef UserDefinedPrimaryGeneratorAction_h
#define UserDefinedPrimaryGeneratorAction_h 1

// Geant4 headers
#include "G4VUserPrimaryGeneratorAction.hh"

// c++ std headers
#include "globals.hh"
#include <vector>
#include <fstream>

class G4GeneralParticleSource;
class G4Event;

class UserDefinedPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    UserDefinedPrimaryGeneratorAction();
    ~UserDefinedPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* anEvent);

  private:
    G4GeneralParticleSource* particleSource;
};
#endif

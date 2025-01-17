
// classes to include from the Geant4 framework
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"

// user defined classes to include
#include "UserDefinedPrimaryGeneratorAction.hh"

// call parent constructor and empty initialize the particle source
// using the initializer list
UserDefinedPrimaryGeneratorAction::UserDefinedPrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction(),
    particleSource(new G4GeneralParticleSource())
{}

// free the allocated memory
UserDefinedPrimaryGeneratorAction::~UserDefinedPrimaryGeneratorAction()
{
  delete particleSource;
}

void UserDefinedPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  particleSource->GeneratePrimaryVertex(anEvent);
}

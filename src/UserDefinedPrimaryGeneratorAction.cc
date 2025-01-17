
// classes to include from the Geant4 framework
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"

// joh
// from: https://apc.u-paris.fr/~franco/g4doxy4.10/html/_g4_eta_prime_8cc_source.html
#include "G4EtaPrime.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4PhaseSpaceDecayChannel.hh"
#include "G4DalitzDecayChannel.hh"
#include "G4DecayTable.hh"
   
// user defined classes to include
#include "UserDefinedPrimaryGeneratorAction.hh"

// call parent constructor and empty initialize the particle source
// using the initializer list
UserDefinedPrimaryGeneratorAction::UserDefinedPrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction(),
    particleSource(new G4GeneralParticleSource())
{
}

// free the allocated memory
UserDefinedPrimaryGeneratorAction::~UserDefinedPrimaryGeneratorAction()
{
  delete particleSource;
}

void UserDefinedPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  particleSource->GeneratePrimaryVertex(anEvent);
}

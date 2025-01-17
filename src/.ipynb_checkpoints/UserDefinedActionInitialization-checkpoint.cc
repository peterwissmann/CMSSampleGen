
// local headers to include
#include "UserDefinedActionInitialization.hh"
#include "UserDefinedPrimaryGeneratorAction.hh"

// classes to include from the Geant4 framework
#include "G4VUserActionInitialization.hh"

UserDefinedActionInitialization::UserDefinedActionInitialization()
: G4VUserActionInitialization()
{}

UserDefinedActionInitialization::~UserDefinedActionInitialization()
{}

void UserDefinedActionInitialization::Build() const
{
  SetUserAction(new UserDefinedPrimaryGeneratorAction);
}

#ifndef UserDefinedActionInitialization_h
#define UserDefinedActionInitialization_h 1

// classes to include from the Geant4 framework
#include "G4VUserActionInitialization.hh"

class UserDefinedActionInitialization : public G4VUserActionInitialization
{
  public:
    UserDefinedActionInitialization();
    virtual ~UserDefinedActionInitialization();

    // this is called, when run is initialized
    // the "const" in the end is required by G4VUserActionInitialization
    // and prevents Build from changing anything in the object
    virtual void Build() const;
};

#endif

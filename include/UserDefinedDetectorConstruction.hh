
#ifndef UserDefinedDetectorConstruction_h
#define UserDefinedDetectorConstruction_h 1

// classes to include from the Geant4 framework
#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "DetectorMessenger.hh"

// use classes without importing it yet - forward definition
class G4VPhysicalVolume;

class UserDefinedDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    UserDefinedDetectorConstruction();
    virtual ~UserDefinedDetectorConstruction();

    void DefineMaterials();
    virtual G4VPhysicalVolume* Construct();
    void SetIronThickness(G4double thickness);
    void SetIronPosition(G4double position);

  private:
    // members concerning the calorimeter
    G4double caloSizeXY;
    G4double activeMaterialThickness;
    G4int nLayers;
    G4double ironThickness;
    G4double IronPosition;
    DetectorMessenger* fMessenger;
};
#endif

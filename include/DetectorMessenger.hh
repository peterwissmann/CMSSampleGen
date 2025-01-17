#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class UserDefinedDetectorConstruction;

class DetectorMessenger : public G4UImessenger
{
public:
  DetectorMessenger(UserDefinedDetectorConstruction* detector);
  ~DetectorMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
  UserDefinedDetectorConstruction* fDetector;
  G4UIcmdWithADoubleAndUnit* fIronThicknessCmd;
  G4UIcmdWithADoubleAndUnit* fIronPositionCmd;
};

#endif
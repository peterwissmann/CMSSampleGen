#include "DetectorMessenger.hh"
#include "UserDefinedDetectorConstruction.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

DetectorMessenger::DetectorMessenger(UserDefinedDetectorConstruction* detector)
: G4UImessenger(), fDetector(detector)
{
  // Create a command to set the iron thickness
  fIronThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/ironThickness", this);
  fIronThicknessCmd->SetGuidance("Set the thickness of the iron layer.");
  fIronThicknessCmd->SetParameterName("Thickness", false);
  fIronThicknessCmd->SetUnitCategory("Length");
  fIronThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Command to set the iron position
  fIronPositionCmd = new G4UIcmdWithADoubleAndUnit("/detector/ironPosition", this);
  fIronPositionCmd->SetGuidance("Set the position of the iron layer.");
  fIronPositionCmd->SetParameterName("Position", false);
  fIronPositionCmd->SetUnitCategory("Length");
  fIronPositionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete fIronThicknessCmd;
  delete fIronPositionCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fIronThicknessCmd)
  {
    fDetector->SetIronThickness(fIronThicknessCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fIronPositionCmd) // Handle position command
  {
    fDetector->SetIronPosition(fIronPositionCmd->GetNewDoubleValue(newValue));
  }
}
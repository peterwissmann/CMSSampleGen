
// classes to include from the Geant4 framework
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4ScoringManager.hh"
#include "FTFP_BERT.hh"

// user defined classes to include
#include "UserDefinedDetectorConstruction.hh"
#include "UserDefinedActionInitialization.hh"

// stl libraries
#include <unistd.h>

int main(int argc, char** argv){

  //choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine());
  G4long seed = time(NULL);
	pid_t pid = getpid();
	seed *= pid << 16;
  G4Random::setTheSeed(seed);

  // create instance that sets up the simulation and guides the run
  G4RunManager* runManager = new G4RunManager();

  // world and detector setup for the simulation
  runManager->SetUserInitialization(new UserDefinedDetectorConstruction);

  // which physics processes should be used for the simulation
  runManager->SetUserInitialization(new FTFP_BERT);

  // actions during the simulation including the generation of particles
  runManager->SetUserInitialization(new UserDefinedActionInitialization);

  // set score writer to scoring manager
  G4ScoringManager::GetScoringManager();

  if (argc == 1){
    G4cout << "=========================================" << G4endl;
    G4cout << "== No macro passed to run, terminating ==" << G4endl;
    G4cout << "=========================================" << G4endl;
    return 1;
  }

  // run a macro file
  G4String command = "/control/execute ";
  G4String fileName = argv[1];

  if (argc == 3)
  {
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    G4UImanager::GetUIpointer()->ApplyCommand(command+fileName);
    ui->SessionStart();
    delete ui;
    delete visManager;
  }
  else
  {
    G4UImanager::GetUIpointer()->ApplyCommand(command+fileName);
  }

  delete runManager;
  return 0;
}

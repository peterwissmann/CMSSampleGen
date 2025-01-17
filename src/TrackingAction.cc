//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// -------------------------------------------------------------------
// -------------------------------------------------------------------

#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4Region.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "UserDefinedDetectorConstruction.hh"

using namespace std;

//TrackingAction::TrackingAction(UserDefinedDetectorConstruction* detector)
TrackingAction::TrackingAction()
{
  //    fDetector = detector;
    fTargetRegion = 0;
}

TrackingAction::~TrackingAction()
{
  //    fDetector = 0;
    fTargetRegion = 0;
}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    const G4ParticleDefinition* particleDefinition = track->GetParticleDefinition();

    G4int parentID = track->GetParentID();
    G4VPhysicalVolume * trackVol = track->GetVolume();

    if (trackVol->GetName() == "WorldPV") {
      double px = track->GetMomentum().x();
      double py = track->GetMomentum().y();
      double pz = track->GetMomentum().z();
      double pT = sqrt(px*px+py*py);
      double p = sqrt(pT*pT + pz*pz);
      double angle_from_z_axis = asin(pT/p)*360./2./3.1415;
      G4cout << "parent = " << parentID
             << " | ID = " << track->GetTrackID()
             << " | name = " << particleDefinition->GetParticleName()
             << " | E = " << track->GetKineticEnergy()
             << " | px = " << track->GetMomentum().x()
             << " | py = " << track->GetMomentum().y()
             << " | pz = " << track->GetMomentum().z()
             << " | angle = " << angle_from_z_axis << G4endl;
    }
}


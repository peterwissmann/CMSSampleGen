
// local headers to include
#include "UserDefinedDetectorConstruction.hh"

// classes to include from the Geant4 framework
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"


// a constructor that does nothing except call its parent's constructor
UserDefinedDetectorConstruction::UserDefinedDetectorConstruction()
: G4VUserDetectorConstruction()
{
  ////////////////////////////////////////////////////////////////////////////
  // define all sizes as full sizes, setting half values happens in the methods!
  ////////////////////////////////////////////////////////////////////////////

  activeMaterialThickness = 4*mm;
  passiveMaterialThickness = 2*mm;
  nLayers = 80;
  caloSizeXY = 100*cm;
}

// empty destructor
UserDefinedDetectorConstruction::~UserDefinedDetectorConstruction()
{}

void UserDefinedDetectorConstruction::DefineMaterials()
{
  // Define and build materials so that they can be used in the detector construction

  // Lead material defined using NIST Manager
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");

  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;

  // Definition of liquid argon and "vacuum"
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density=1.390*g/cm3);
  new G4Material("Galactic", z=1., a=1.01*g/mole, density=universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);
}

// this method is called when the run is initialized and builds the simulation world
G4VPhysicalVolume* UserDefinedDetectorConstruction::Construct()
{
  DefineMaterials();

  // intermediate result: how thick will the whole calorimeter be?
  G4double layerThickness = activeMaterialThickness+passiveMaterialThickness;
  G4double caloSizeZ = layerThickness*nLayers;

  /////////////////////////////////////////////////////////////////////////////
  // Build the world
  /////////////////////////////////////////////////////////////////////////////

  G4double worldXY = 1.2*caloSizeXY;
  G4double worldZ = 10*m;
  // create a box that will be the world volume
  G4VSolid* worldBox = new G4Box("WorldBox", worldXY/2., worldXY/2., worldZ/2.);
  // attach a material to the world
  G4LogicalVolume* worldLV = new G4LogicalVolume(
    worldBox, G4Material::GetMaterial("Galactic"), "WorldLV"
  );
  // put the box into the realm --> keep as variable worldPV for further reference
  G4VPhysicalVolume* worldPV = new G4PVPlacement(
    0, G4ThreeVector(0,0,0), worldLV, "WorldPV", 0, false, 0, true
  );

  /////////////////////////////////////////////////////////////////////////////
  // Build the calorimeter
  /////////////////////////////////////////////////////////////////////////////

  // bounding box for calorimeter --> we will fill it up in the next step
  G4VSolid* caloBox = new G4Box("CaloBox", caloSizeXY/2., caloSizeXY/2., caloSizeZ/2.);
  G4LogicalVolume* caloLV = new G4LogicalVolume(
    caloBox, G4Material::GetMaterial("Galactic"), "CaloLV"
  );
  new G4PVPlacement(
    0, G4ThreeVector(0,0,0), caloLV, "CaloPV", worldLV, false, 0, true
  );

  // container for the individual layers --> replicate within bounding box
  G4VSolid* layerBox = new G4Box("LayerBox", caloSizeXY/2, caloSizeXY/2, layerThickness/2);
  G4LogicalVolume* layerLV = new G4LogicalVolume(
    layerBox, G4Material::GetMaterial("Galactic"), "LayerLV"
  );
  // no simple placement but instead place as replicating volume into the calo
  new G4PVReplica(
    "LayerPV", layerLV, caloLV, kZAxis, nLayers, layerThickness
  );

  //
  // Passive layer
  //
  G4VSolid* passiveBox = new G4Box("PassiveBox", caloSizeXY/2, caloSizeXY/2, passiveMaterialThickness/2);
  G4LogicalVolume* passiveLV = new G4LogicalVolume(
    passiveBox, G4Material::GetMaterial("G4_Pb"), "PassiveLV"
  );
  // put an passive layer part into the layer container --> passive layer in each replica!
  new G4PVPlacement(
    0, G4ThreeVector(0., 0., -passiveMaterialThickness/2), // fill into bottom part of calo
    passiveLV, "PassivePV", layerLV, false, 0, true
  );

  //
  // Active layer
  //
  G4VSolid* activeBox = new G4Box("ActiveBox", caloSizeXY/2, caloSizeXY/2, activeMaterialThickness/2);
  G4LogicalVolume* activeLV = new G4LogicalVolume(
    activeBox, G4Material::GetMaterial("liquidArgon"), "ActiveLV"
  );
  // put an active layer part into the layer container --> active layer in each replica!
  new G4PVPlacement(
    0, G4ThreeVector(0., 0., activeMaterialThickness/2), // fill into top part of calo
    activeLV, "ActivePV", layerLV, false, 0, true
  );


  /////////////////////////////////////////////////////////////////////////////
  // Some visual settings
  /////////////////////////////////////////////////////////////////////////////

  worldLV->SetVisAttributes (G4VisAttributes::Invisible);

  G4VisAttributes* baseVisAttr = new G4VisAttributes();
  baseVisAttr->SetVisibility(true);
  activeLV->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));
  passiveLV->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

  G4VisAttributes* containerVisAttr = new G4VisAttributes();
  containerVisAttr->SetVisibility(false);
  layerLV->SetVisAttributes(containerVisAttr);
  caloLV->SetVisAttributes(containerVisAttr);
  worldLV->SetVisAttributes(containerVisAttr);

  return worldPV;
}

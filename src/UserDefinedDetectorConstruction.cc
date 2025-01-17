
// local headers to include
#include "UserDefinedDetectorConstruction.hh"
#include "MagneticField.hh"

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

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ClassicalRK4.hh"

// a constructor that does nothing except call its parent's constructor
UserDefinedDetectorConstruction::UserDefinedDetectorConstruction()
: G4VUserDetectorConstruction(),
fMessenger(new DetectorMessenger(this)), // Initialize messenger
ironThickness(0.0*mm),
IronPosition(0.0 * cm)
{
  ////////////////////////////////////////////////////////////////////////////
  // define all sizes as full sizes, setting half values happens in the methods!
  ////////////////////////////////////////////////////////////////////////////

  activeMaterialThickness = 230*mm;
  nLayers = 1;
  caloSizeXY = 100*cm;
}

// empty destructor
UserDefinedDetectorConstruction::~UserDefinedDetectorConstruction()
{
  delete fMessenger;
}

//Variable Iron thickness
void UserDefinedDetectorConstruction::SetIronThickness(G4double thickness)
{
  ironThickness = thickness;
}

void UserDefinedDetectorConstruction::SetIronPosition(double position)
{
    IronPosition = position; // Assuming fIronPosition is a member variable
}

void UserDefinedDetectorConstruction::DefineMaterials()
{
  // Define and build materials so that they can be used in the detector construction

  // Lead material defined using NIST Manager
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_PbWO4");
  nistManager->FindOrBuildMaterial("G4_Fe");
  
  // "vacuum" material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;

  // Definition of "vacuum"
  new G4Material("Galactic", z=1., a=1.01*g/mole, density=universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);
}

// this method is called when the run is initialized and builds the simulation world
G4VPhysicalVolume* UserDefinedDetectorConstruction::Construct()
{
  DefineMaterials();

  // intermediate result: how thick will the whole calorimeter be?
  G4double layerThickness = activeMaterialThickness;
  G4double caloSizeZ = layerThickness;

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
  
  //Iron layer
  // Iron layer dimensions and position
  G4Material* ironMaterial = G4Material::GetMaterial("G4_Fe");

  //Higer density iron:
  // Increase density by 20%
  G4double originalDensity = ironMaterial->GetDensity();
  G4double increasedDensity = 1.25 * originalDensity;
  // Clone the material with a new density
  G4Material* modifiedIronMaterial = new G4Material("ModifiedIron", increasedDensity, 
                                                    ironMaterial->GetNumberOfElements());
  for (size_t i = 0; i < ironMaterial->GetNumberOfElements(); ++i) {
      G4Element* element = const_cast<G4Element*>(ironMaterial->GetElement(i));
      G4double fraction = ironMaterial->GetFractionVector()[i];
      modifiedIronMaterial->AddElement(element, fraction);
  }

  G4double ironRadLength = 0.0;
  ironRadLength = ironMaterial->GetRadlen();
  //G4double ironThickness = ironRadLength;
  G4double sourceZPos = -1.405 * m;
  G4double detectorZPos = 0.0 * m;
  G4double ironZPos = (sourceZPos + detectorZPos) / 2.0;  // Midpoint between source and detector

  G4VSolid* ironBox = new G4Box("IronBox", caloSizeXY / 2., caloSizeXY / 2., ironThickness / 2.);
  G4LogicalVolume* ironLV = new G4LogicalVolume(ironBox, modifiedIronMaterial, "IronLV"); //Hier wieder auf Eisen zurückändern!

  // Place the iron layer at half way to detector
  new G4PVPlacement(0, G4ThreeVector(0, 0, IronPosition), ironLV, "IronPV", worldLV, false, 0, true);

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
  // Active layer
  //
  G4VSolid* activeBox = new G4Box("ActiveBox", caloSizeXY/2, caloSizeXY/2, activeMaterialThickness/2);
  G4LogicalVolume* activeLV = new G4LogicalVolume(
    activeBox, G4Material::GetMaterial("G4_PbWO4"), "ActiveLV"
  );
  
  // put an active layer part into the layer container --> active layer in each replica!
  new G4PVPlacement(
    0, G4ThreeVector(0., 0., 0.), // fill into top part of calo
    activeLV, "ActivePV", layerLV, false, 0, true
  );

  ///////////////////////////////////////////////////////////////////////////
  // Magnetic field implementation in the world volume
  ///////////////////////////////////////////////////////////////////////////

  // 1. Create a magnetic field object
  G4MagneticField* magField = new MagneticField();

  // 2. Create a field manager and associate it with the transportation manager
  G4FieldManager* fieldManager = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  fieldManager->SetDetectorField(magField);

  // 3. Create the equation of motion for particles in the magnetic field
  G4Mag_UsualEqRhs* equation = new G4Mag_UsualEqRhs(magField);

  // 4. Choose a stepper method for the integration (Classical Runge-Kutta here)
  G4MagIntegratorStepper* stepper = new G4ClassicalRK4(equation);

  // 5. Create a chord finder for the field manager, defining the precision for particle motion
  G4ChordFinder* chordFinder = new G4ChordFinder(magField, 1.0e-2 * mm, stepper);
  fieldManager->SetChordFinder(chordFinder);


  /////////////////////////////////////////////////////////////////////////////
  // Some visual settings
  /////////////////////////////////////////////////////////////////////////////

  //worldLV->SetVisAttributes (G4VisAttributes::Invisible);

  //G4VisAttributes* baseVisAttr = new G4VisAttributes();
  //baseVisAttr->SetVisibility(true);
  //activeLV->SetVisAttributes(G4VisAttributes(G4Colour::Magenta()));

  //G4VisAttributes* containerVisAttr = new G4VisAttributes();
  //containerVisAttr->SetVisibility(false);
  //layerLV->SetVisAttributes(containerVisAttr);
  //caloLV->SetVisAttributes(containerVisAttr);
  //worldLV->SetVisAttributes(containerVisAttr);
  // Set visualization attributes for the calorimeter volume
G4VisAttributes* caloVisAttr = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.2)); // light gray, semi-transparent
caloVisAttr->SetVisibility(true);
caloLV->SetVisAttributes(caloVisAttr);

// Set visualization attributes for the layer volume
G4VisAttributes* layerVisAttr = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.3)); // blue, semi-transparent
layerVisAttr->SetVisibility(true);
layerLV->SetVisAttributes(layerVisAttr);

// Set visualization attributes for the active material volume
G4VisAttributes* activeVisAttr = new G4VisAttributes(G4Colour::Magenta()); // magenta, fully opaque
activeVisAttr->SetVisibility(true);
activeLV->SetVisAttributes(activeVisAttr);

  return worldPV;
}

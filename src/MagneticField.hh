#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

class MagneticField : public G4MagneticField {
public:
    MagneticField() {}
    virtual ~MagneticField() {}

    // Define magnetic field vector (e.g., along Z-axis)
    virtual void GetFieldValue(const G4double[4], G4double* field) const override {
        field[0] = 0.0 * tesla;  // x-component of B field
        field[1] = 4.0 * tesla;  // y-component of B field
        field[2] = 0.0 * tesla; // z-component of B field
    }
};
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
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "G4RunManager.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Paraboloid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4AutoDelete.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4SDManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4SDChargedFilter.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fCheckOverlaps(true)
  {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

DetectorConstruction::~DetectorConstruction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4VPhysicalVolume* DetectorConstruction::Construct(){

    //** World **//
    G4Material* Galactic = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

    //G4double worldSize = 1. * CLHEP::meter;
    G4double worldSize = 1. * m;

    G4Box* worldSolid = new G4Box("World",
                                  worldSize/2.,
                                  worldSize/2.,
                                  worldSize/2.);

    G4LogicalVolume* worldLogic = new G4LogicalVolume(worldSolid,
                                                      Galactic,
                                                      "World");

    G4PVPlacement* worldPhysical = new G4PVPlacement(0,
                                                     G4ThreeVector(),
                                                     worldLogic,
                                                     "World",
                                                     0,
                                                     false,
                                                     0);


    //** TODO: Insert the material definition here **//

G4Material* Ge_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ge");



    //Print all the materials defined
  	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
    G4cout << "###############################" << G4endl;


    //***********//


    //** TODO: Insert the detector geometry here **//

    G4double crySize_z    =  1.*cm;
    G4double crySize_mDz  =  18.*cm;
    G4double crySize_pDz  =  20.*cm;

    G4Paraboloid* crySolid = new G4Paraboloid("cry",
                                              crySize_z/2.,     //Half length Z
                                              crySize_mDz/2.,   //Radius at -Dz
                                              crySize_pDz/2.);  //Radius at +Dz greater than R1

    //G4Tubs* crySolid = new G4Tubs("cry",
    //  10.*cm,   // Inner radius
    //  15.*cm,   // Outer radius
    //  20.*cm,     // Half length in z
    //  0.*rad,   // Starting phi angle in radians
    //  2.*pi*rad);  // Angle of the segment in radians


    G4LogicalVolume* cryLogic = new G4LogicalVolume(crySolid,   //its solid
                                                    Ge_mat,     //its material
                                                    "cryLV");     //its name



  // Distance from the center

  G4double r_0 = 40.*cm;

  //define a rotation matrix
  G4double tetha  =   pi/2.*rad;
  G4double phi    =   pi/4.*rad;

	G4RotationMatrix* myRotationMatrix = new G4RotationMatrix();
	myRotationMatrix ->  rotateX(tetha/2.);
  myRotationMatrix ->  rotateY(-tetha/2.);
  myRotationMatrix ->  rotateZ(phi);


  // define a translation vector
  G4ThreeVector posCry = G4ThreeVector( r_0*sin(tetha)*cos(phi),
                                        r_0*sin(tetha)*sin(phi),
                                        r_0*cos(phi) );

  new G4PVPlacement(  myRotationMatrix,             //no rotation
                      posCry,         //at (0,0,0)
                      cryLogic,                //its logical volume
                      "cry",              //its name
                      worldLogic,              //its mother  volume
                      false,                   //no boolean operation
                      0);                       //copy number



    //***********//

    return worldPhysical;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField(){

    //** TODO: Insert the sensitive detectors (SD) here **//
    //
    // Scorers
    //

    // declare Absorber as a MultiFunctionalDetector scorer
    //
    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    auto cryDetector = new G4MultiFunctionalDetector("Crystal");
    G4SDManager::GetSDMpointer()->AddNewDetector(cryDetector);

    G4VPrimitiveScorer* primitive;
    primitive = new G4PSEnergyDeposit("Edep");
    cryDetector->RegisterPrimitive(primitive);

    primitive = new G4PSTrackLength("TrackLength");
    auto charged = new G4SDChargedFilter("chargedFilter");
    primitive ->SetFilter(charged);
    cryDetector->RegisterPrimitive(primitive);

    SetSensitiveDetector("cryLV",cryDetector);
    //***********//
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo..

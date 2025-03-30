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
/// \file persistency/gdml/U01/load_gdml.cc
/// \brief Main program of the persistency/gdml/U01 example
//
//
//
//
// --------------------------------------------------------------
//      GEANT 4 - load_gdml
//
// --------------------------------------------------------------

#include "FTFP_BERT.hh"
#include "U01ActionInitialization.hh"
#include "U01DetectorConstruction.hh"
#include "U01PrimaryGeneratorAction.hh"

#include "G4USDParser.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4RunManagerFactory.hh"
#include "G4TransportationManager.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

#include <vector>

// --------------------------------------------------------------

int main(int argc, char** argv)
{
  G4USDParser parser;
  parser.Read(G4String(argv[1]));
  parser.Write(G4String(argv[2]),parser.GetWorldVolume());

  auto* runManager = G4RunManagerFactory::CreateRunManager();

  auto wp =parser.GetWorldVolume();

  runManager->SetUserInitialization(new U01DetectorConstruction(parser.GetWorldVolume()));
  runManager->SetUserInitialization(new FTFP_BERT);
  runManager->SetUserInitialization(new U01ActionInitialization());

  G4cout << "G4RunManager : init" << G4endl;
  runManager->Initialize();

  // Initialize visualization
  G4cout << "G4VisExecutive : init" << G4endl;
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  runManager->BeamOn(0);

  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  UImanager->ApplyCommand("/control/execute vis.mac");
  ui->SessionStart();
  delete ui;

  delete visManager;
  delete runManager;

  return 0;
}
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
// G4USDParser
//
// Class description:
//
// USD main parser.

// Author: Stewart Boogert 2024
// --------------------------------------------------------------------

#ifndef G4USDREADER_HH
#define G4USDREADER_HH

#include <map>

#include "G4String.hh"

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>

class G4Box;
class G4DisplacedSolid;
class G4UnionSolid;
class G4IntersectionSolid;
class G4SubtractionSolid;
class G4MultiUnion;
class G4PVPlacement;

class G4USDReader {
  public:
    G4USDReader(const G4String& filename);
    ~G4USDReader() = default;
    void ProcessStage(pxr::UsdStagePtr stage);

    G4VPhysicalVolume *GetWorldVolume();

    G4Box* MakeBox(pxr::UsdPrim &prim);
    G4DisplacedSolid* MakeDisplacedSolid(pxr::UsdPrim &prim);
    G4UnionSolid* MakeUnionSolid(pxr::UsdPrim &prim);
    G4IntersectionSolid* MakeIntersectionSolid(pxr::UsdPrim &prim);
    G4SubtractionSolid* MakeSubtractionSolid(pxr::UsdPrim &prim);
    G4MultiUnion* MakeMultiUnion(pxr::UsdPrim &prim);

    G4LogicalVolume* MakeLogicalVolume(pxr::UsdPrim &prim);
    G4PVPlacement* MakePVPlacement(pxr::UsdPrim &prim);

  private:
    G4LogicalVolume *worldLV;

    std::map<G4String, pxr::UsdPrim> allPrimsMap;
    std::map<G4String, std::any> allGeant4Map;
};

#endif  // G4USDREADER_HH

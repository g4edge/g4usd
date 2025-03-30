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
// G4USDWriter
//
// Class description:
//
// USD writer.

// Author: Stewart Boogert 2024
// --------------------------------------------------------------------

#ifndef G4USDWRITE_HH
#define G4USDWRITE_HH 1

class G4String;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VSolid;
class G4Box;
class G4DisplacedSolid;
class G4SubtractionSolid;
class G4UnionSolid;
class G4IntersectionSolid;
class G4MultiUnion;

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>

class G4USDWriter {
  public:
    G4USDWriter(const G4String& filename, G4LogicalVolume *lv_top);
    ~G4USDWriter() = default;

    void ProcessLogicalVolumeTop(const pxr::UsdStagePtr &stage, G4LogicalVolume *lv_top);
    void ProcessPhysicalVolumeTop(const pxr::UsdStagePtr &stage, G4VPhysicalVolume *pv_top);
    void ProcessLogicalVolume(const pxr::UsdStagePtr &stage, pxr::UsdPrim &prim, G4LogicalVolume *lv);
    void ProcessPhysicalVolume(const pxr::UsdStagePtr &stage, pxr::UsdPrim &prim, G4VPhysicalVolume *pv);
    G4String ProcessSolid(const pxr::UsdStagePtr &stage, pxr::UsdPrim &prim, G4VSolid *solid);
    G4String ProcessBox(const pxr::UsdStagePtr &stage, pxr::UsdPrim &prim, G4Box *box);
    G4String ProcessDisplaced(const pxr::UsdStagePtr &stage, pxr::UsdPrim &prin, G4DisplacedSolid *displaced);
    G4String ProcessSubtraction(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4SubtractionSolid* solid);
    G4String ProcessUnion(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4UnionSolid* solid);
    G4String ProcessIntersection(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4IntersectionSolid* solid);
    G4String ProcessMultiUnion(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4MultiUnion* solid);
};

#endif

#include <iostream>
#include <vector>

#include "G4String.hh"
#include "G4Box.hh"
#include "G4DisplacedSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4MultiUnion.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"

#include "G4USDReader.hh"

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/attribute.h>

G4USDReader::G4USDReader(const G4String& filename) : worldLV(nullptr) {
  auto stage = pxr::UsdStage::Open(filename);

  ProcessStage(stage);
}

void G4USDReader::ProcessStage(pxr::UsdStagePtr stage) {

  // traverse all prims top to bottom
  std::vector<pxr::UsdPrim> allPrims;
  for (auto prim : stage->Traverse()) {
    allPrims.push_back(prim);

    allPrimsMap[G4String(prim.GetPath().GetAsString())] = prim;
  }

  // reverse traversal
  std::reverse(allPrims.begin(), allPrims.end());

  // traverse bottom to top and create geant4 objects
  for(auto prim : allPrims) {
    std::cout << prim.GetPath().GetAsString() << " " << prim.GetTypeName() << std::endl;
    if(prim.GetTypeName() == "Box") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = dynamic_cast<G4VSolid*>(MakeBox(prim));
    }
    else if(prim.GetTypeName() == "DisplacedSolid") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = dynamic_cast<G4VSolid*>(MakeDisplacedSolid(prim));
    }
    else if(prim.GetTypeName() == "Subtraction") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = dynamic_cast<G4VSolid*>(MakeSubtractionSolid(prim));
    }
    else if(prim.GetTypeName() == "Union") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = dynamic_cast<G4VSolid*>(MakeUnionSolid(prim));
    }
    else if(prim.GetTypeName() == "Intersection") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = dynamic_cast<G4VSolid*>(MakeIntersectionSolid(prim));
    }
    else if(prim.GetTypeName() == "MultiUnion") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = dynamic_cast<G4VSolid*>(MakeMultiUnion(prim));
    }
    else if(prim.GetTypeName() == "Logical") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = MakeLogicalVolume(prim);
    }
    else if(prim.GetTypeName() == "Placement") {
      allGeant4Map[G4String(prim.GetPath().GetAsString())] = MakePVPlacement(prim);
    }
    else {
      std::cout << "G4USDReader::ProcessStage prim not found" << std::endl;
    }
  }

  // store world volume for later
  auto rootPrimPath = (*stage->Traverse().begin()).GetPath().GetString();
  worldLV = std::any_cast<G4LogicalVolume*>(allGeant4Map[rootPrimPath]);
}


G4VPhysicalVolume* G4USDReader::GetWorldVolume() {

  auto pv_world = new G4PVPlacement(nullptr,
                                   G4ThreeVector(0, 0, 0),
                                   worldLV,
                                   worldLV->GetName()+"_PV",
                                    nullptr, false, 0);

  return pv_world;
}

G4Box* G4USDReader::MakeBox(pxr::UsdPrim &prim) {
  std::cout << "G4USDReader::MakeBox" << std::endl;

  G4double dx;
  G4double dy;
  G4double dz;

  prim.GetAttribute(pxr::TfToken("x")).Get(&dx);
  prim.GetAttribute(pxr::TfToken("y")).Get(&dy);
  prim.GetAttribute(pxr::TfToken("z")).Get(&dz);

  auto *box = new G4Box(prim.GetName().GetString(), dx, dy, dz);

  return box;
}

G4DisplacedSolid* G4USDReader::MakeDisplacedSolid(pxr::UsdPrim &prim) {
  std::cout << "G4USDReader::MakeDisplacedSolid" << std::endl;

  pxr::GfVec3d translation = pxr::GfVec3d();
  pxr::GfVec3d rotation = pxr::GfVec3d();

  prim.GetAttribute(pxr::TfToken("translation")).Get(&translation);
  prim.GetAttribute(pxr::TfToken("rotation")).Get(&rotation);

  G4RotationMatrix *rotation_g4 = new G4RotationMatrix();
  G4ThreeVector translaton_g4 = G4ThreeVector(translation[0],translation[1], translation[2]);

  rotation_g4->rotateX(rotation[0]/180*M_PI);
  rotation_g4->rotateY(rotation[1]/180*M_PI);
  rotation_g4->rotateZ(rotation[2]/180*M_PI);
  rotation_g4->rectify();

  G4DisplacedSolid *displaced = new G4DisplacedSolid(G4String(prim.GetName().GetString()),
                                                     std::any_cast<G4VSolid*>(allGeant4Map[prim.GetChildren().begin()->GetPath().GetAsString()]),
                                                     rotation_g4,
                                                     translaton_g4);

  return displaced;
}

G4UnionSolid* G4USDReader::MakeUnionSolid(pxr::UsdPrim &prim) {
  std::cout << "G4USDReader::MakeUnionSolid" << std::endl;

  std::string solid1Name;
  std::string solid2Name;

  prim.GetAttribute(pxr::TfToken("solid1prim")).Get(&solid1Name);
  prim.GetAttribute(pxr::TfToken("solid2prim")).Get(&solid2Name);

  auto solid1_prim = prim.GetChild(pxr::TfToken(solid1Name));
  auto solid2_prim = prim.GetChild(pxr::TfToken(solid2Name));

  G4VSolid *solid1 = std::any_cast<G4VSolid*>(allGeant4Map[solid1_prim.GetPath().GetAsString()]);
  G4DisplacedSolid *solid2 = dynamic_cast<G4DisplacedSolid*>(std::any_cast<G4VSolid*>(allGeant4Map[solid2_prim.GetPath().GetAsString()]));

  G4UnionSolid *unionSolid = new G4UnionSolid(G4String(prim.GetName()),
                                                       solid1,
                                                       solid2->GetConstituentMovedSolid(),
                                                       new G4RotationMatrix(solid2->GetObjectRotation()),
                                                       solid2->GetObjectTranslation());

  return unionSolid;
}

G4IntersectionSolid* G4USDReader::MakeIntersectionSolid(pxr::UsdPrim &prim) {
  std::cout << "G4USDReader::MakeIntersectionSolid" << std::endl;

  std::string solid1Name;
  std::string solid2Name;

  prim.GetAttribute(pxr::TfToken("solid1prim")).Get(&solid1Name);
  prim.GetAttribute(pxr::TfToken("solid2prim")).Get(&solid2Name);

  auto solid1_prim = prim.GetChild(pxr::TfToken(solid1Name));
  auto solid2_prim = prim.GetChild(pxr::TfToken(solid2Name));

  G4VSolid *solid1 = std::any_cast<G4VSolid*>(allGeant4Map[solid1_prim.GetPath().GetAsString()]);
  G4DisplacedSolid *solid2 = dynamic_cast<G4DisplacedSolid*>(std::any_cast<G4VSolid*>(allGeant4Map[solid2_prim.GetPath().GetAsString()]));

  G4IntersectionSolid *intersection = new G4IntersectionSolid(G4String(prim.GetName()),
                                                              solid1,
                                                              solid2->GetConstituentMovedSolid(),
                                                              new G4RotationMatrix(solid2->GetObjectRotation()),
                                                              solid2->GetObjectTranslation());

  return intersection;
}

G4SubtractionSolid* G4USDReader::MakeSubtractionSolid(pxr::UsdPrim &prim) {
  std::cout << "G4USDReader::MakeSubtractionSolid" << std::endl;

  std::string solid1Name;
  std::string solid2Name;

  prim.GetAttribute(pxr::TfToken("solid1prim")).Get(&solid1Name);
  prim.GetAttribute(pxr::TfToken("solid2prim")).Get(&solid2Name);

  auto solid1_prim = prim.GetChild(pxr::TfToken(solid1Name));
  auto solid2_prim = prim.GetChild(pxr::TfToken(solid2Name));

  G4VSolid *solid1 = std::any_cast<G4VSolid*>(allGeant4Map[solid1_prim.GetPath().GetAsString()]);
  G4DisplacedSolid *solid2 = dynamic_cast<G4DisplacedSolid*>(std::any_cast<G4VSolid*>(allGeant4Map[solid2_prim.GetPath().GetAsString()]));

  G4SubtractionSolid *subtraction = new G4SubtractionSolid(G4String(prim.GetName()),
                                                           solid1,
                                                           solid2->GetConstituentMovedSolid(),
                                                           new G4RotationMatrix(solid2->GetObjectRotation()),
                                                           solid2->GetObjectTranslation());

  return subtraction;
}

G4MultiUnion* G4USDReader::MakeMultiUnion(pxr::UsdPrim& prim) {
  std::cout << "G4USDReader::MakeMultiUnion name="
            << prim.GetName().GetString() << std::endl;

  pxr::VtArray<std::string> daughterNames;
  prim.GetAttribute(pxr::TfToken("solidprims")).Get(&daughterNames);

  auto mu = new G4MultiUnion(prim.GetName().GetString());

  for(size_t i = 0; i< daughterNames.size(); i++) {
    auto child_i = prim.GetChild(pxr::TfToken(daughterNames[i]));
    auto *solid_i = dynamic_cast<G4DisplacedSolid*>(std::any_cast<G4VSolid*>(allGeant4Map[child_i.GetPath().GetAsString()]));
    mu->AddNode(solid_i->GetConstituentMovedSolid(), solid_i->GetTransform());
  }

  return mu;
}

G4LogicalVolume* G4USDReader::MakeLogicalVolume(pxr::UsdPrim& prim) {
  std::cout << "G4USDReader::MakeLogicalVolume name="
            << prim.GetName().GetString() << std::endl;

  std::string solidName;
  pxr::VtArray<std::string> daughterNames;

  prim.GetAttribute(pxr::TfToken("solidprim")).Get(&solidName);
  prim.GetAttribute(pxr::TfToken("daughters")).Get(&daughterNames);

  auto solid_prim = prim.GetChild(pxr::TfToken(solidName));
  auto solid_path = solid_prim.GetPath().GetAsString();
  std::cout << "G4USDReader::MakeLogicalVolume solid="
            <<  solid_path << std::endl;

  G4VSolid *solid = std::any_cast<G4VSolid*>(allGeant4Map[solid_path]);

  G4LogicalVolume *lv = new G4LogicalVolume(solid,
                                            new G4Material("test_mat",1,1,1),
                                            G4String(prim.GetName()));

  // loop over daughter volumes
  std::cout << "G4USDReader::MakeLogicalVolume ndaughters=" << daughterNames.size() << std::endl;
  for(auto dv_name : daughterNames) {
    std::cout << "G4USDReader::MakeLogicalVolume daughter_name=" << dv_name << std::endl;
    auto dv_prim_path = prim.GetChild(pxr::TfToken(dv_name)).GetPath().GetString();
    lv->AddDaughter(std::any_cast<G4PVPlacement*>(allGeant4Map[dv_prim_path]));
  }

  return lv;
}

G4PVPlacement* G4USDReader::MakePVPlacement(pxr::UsdPrim& prim) {
  std::cout << "G4USDReader::MakePVPlacement" << std::endl;

  auto lv = std::any_cast<G4LogicalVolume*>(allGeant4Map[G4String(prim.GetChildren().begin()->GetPath().GetAsString())]);

  pxr::GfVec3d translation;
  pxr::GfVec3d rotation;

  prim.GetAttribute(pxr::TfToken("translation")).Get(&translation);
  prim.GetAttribute(pxr::TfToken("rotation")).Get(&rotation);

  G4RotationMatrix *rotation_g4 = new G4RotationMatrix();
  G4ThreeVector translaton_g4 = G4ThreeVector(translation[0],translation[1], translation[2]);

  rotation_g4->rotateX(rotation[0]);
  rotation_g4->rotateY(rotation[1]);
  rotation_g4->rotateZ(rotation[2]);
  rotation_g4->rectify();

  std::cout << "G4USDReader::MakePVPlacement " << lv << std::endl;

  auto *pv = new G4PVPlacement(rotation_g4,
                               translaton_g4,
                               lv,
                               G4String(prim.GetName().GetString()),
                               nullptr,
                               false,
                               false);

  return pv;
}
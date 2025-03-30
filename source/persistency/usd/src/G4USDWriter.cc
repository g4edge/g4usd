#include <cmath>

#include "G4USDWriter.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4DisplacedSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4MultiUnion.hh"

#include "pxr/usd/usdGeom/mesh.h"

#include "g4/logical.h"
#include "g4/placement.h"
#include "g4/box.h"
#include "g4/displacedSolid.h"
#include "g4/subtraction.h"
#include "g4/union.h"
#include "g4/intersection.h"
#include "g4/multiUnion.h"

G4USDWriter::G4USDWriter(const G4String& filename, G4LogicalVolume *top_lv) {
  // open in memory stage
  auto stage = pxr::UsdStage::CreateInMemory();
  this->ProcessLogicalVolumeTop(stage,top_lv);
  stage->GetRootLayer()->Export(filename);
}

void G4USDWriter::ProcessLogicalVolumeTop(const pxr::UsdStagePtr &stage, G4LogicalVolume *lv_top) {
  std::cout << "G4USDWriter::ProcessLogicalVolumeTop" << std::endl;

  auto lv_pxr = pxr::G4Logical::Define(stage, pxr::SdfPath(G4String("/")+G4String(lv_top->GetName())));
  auto lv_prim = lv_pxr.GetPrim();

  // make solid
  auto solid_name = ProcessSolid(stage, lv_prim , lv_top->GetSolid());
  lv_pxr.GetSolidprimAttr().Set(std::string(solid_name));

  // make daughters
  for(size_t i=0;i<lv_top->GetNoDaughters(); i++) {
    auto dv = lv_top->GetDaughter(i);
    ProcessPhysicalVolume(stage, lv_prim, dv);
  }
}

void G4USDWriter::ProcessPhysicalVolumeTop(const pxr::UsdStagePtr &stage, G4VPhysicalVolume *pv_top) {
  std::cout << "G4USDWriter::ProcessPhysicalVolumeTop" << std::endl;

  auto lv_top = pv_top->GetLogicalVolume();
  auto lv_pxr = pxr::G4Logical::Define(stage, pxr::SdfPath(G4String("/")+G4String(lv_top->GetName())));

}

void G4USDWriter::ProcessLogicalVolume(const pxr::UsdStagePtr &stage, pxr::UsdPrim &prim, G4LogicalVolume *lv) {
  std::cout << "G4USDWriter::ProcessLogicalVolume" << std::endl;

  auto lv_pxr = pxr::G4Logical::Define(stage, prim.GetPath().AppendPath(pxr::SdfPath(lv->GetName())));
  auto lv_prim = lv_pxr.GetPrim();

  // make solid
  auto solid_name = ProcessSolid(stage, lv_prim , lv->GetSolid());
  lv_pxr.GetSolidprimAttr().Set(std::string(solid_name));

  // make daughters
  for(size_t i=0;i<lv->GetNoDaughters(); i++) {
    auto dv = lv->GetDaughter(i);
    ProcessPhysicalVolume(stage, lv_prim, dv);
  }
}

void G4USDWriter::ProcessPhysicalVolume(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4VPhysicalVolume* pv)
{
  auto pv_pxr = pxr::G4Placement::Define(stage, prim.GetPath().AppendPath(pxr::SdfPath(pv->GetName())));
  auto pv_prim = pv_pxr.GetPrim();
  this->ProcessLogicalVolume(stage, pv_prim, pv->GetLogicalVolume());
}

G4String G4USDWriter::ProcessSolid(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4VSolid* solid) {
  std::cout << "G4USDWriter::ProcessSolid" << std::endl;

  if(dynamic_cast<G4Box*>(solid)) {
    return ProcessBox(stage, prim, dynamic_cast<G4Box*>(solid));
  }
  else if(dynamic_cast<G4DisplacedSolid*>(solid)) {
    return ProcessDisplaced(stage, prim, dynamic_cast<G4DisplacedSolid*>(solid));
  }
  else if(dynamic_cast<G4SubtractionSolid*>(solid)) {
    return ProcessSubtraction(stage, prim, dynamic_cast<G4SubtractionSolid*>(solid));
  }
  else if(dynamic_cast<G4UnionSolid*>(solid)) {
    return ProcessUnion(stage, prim, dynamic_cast<G4UnionSolid*>(solid));
  }
  else if(dynamic_cast<G4IntersectionSolid*>(solid)) {
    return ProcessIntersection(stage, prim, dynamic_cast<G4IntersectionSolid*>(solid));
  }
  else if(dynamic_cast<G4MultiUnion*>(solid)) {
    return ProcessMultiUnion(stage, prim, dynamic_cast<G4MultiUnion*>(solid));
  }
  else {
    std::cout << "G4USDWriter::ProcessSolid Cannot process solid" << std::endl;
    return G4String("none");
  }
}

G4String G4USDWriter::ProcessBox(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4Box* solid) {
  std::cout << "G4USDWriter::ProcessBox " << solid->GetName() << std::endl;

  auto solid_path = prim.GetPath().AppendPath(pxr::SdfPath(solid->GetName()));
  auto box = pxr::G4Box::Define(stage,solid_path);
  box.GetXAttr().Set(solid->GetXHalfLength());
  box.GetYAttr().Set(solid->GetYHalfLength());
  box.GetZAttr().Set(solid->GetZHalfLength());
  box.Update();

  return box.GetPrim().GetName().GetString();
}

G4String G4USDWriter::ProcessDisplaced(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4DisplacedSolid* solid) {
  std::cout << "G4USDWriter::ProcessDisplaced " << solid->GetName() << std::endl;

  auto solid_path = prim.GetPath().AppendPath(pxr::SdfPath(solid->GetName()));
  auto displaced = pxr::G4DisplacedSolid::Define(stage,solid_path);
  auto displaced_prim = displaced.GetPrim();

  this->ProcessSolid(stage, displaced_prim, solid->GetConstituentMovedSolid());

  auto displaced_rotation = solid->GetObjectRotation();
  auto displaced_translation = solid->GetObjectTranslation();

  // Extract the components of the rotation matrix
  G4double Rxx = displaced_rotation.xx(), Rxy = displaced_rotation.xy();
  G4double Ryx = displaced_rotation.yx(), Ryy = displaced_rotation.yy();
  G4double Rzx = displaced_rotation.zx(), Rzy = displaced_rotation.zy(), Rzz = displaced_rotation.zz();

  // Compute the Euler angles (in radians)
  G4double thetaY = std::asin(-Rzx);  // Rotation around Y-axis

  G4double cosThetaY = std::cos(thetaY);
  G4double thetaX, thetaZ;

  if (std::fabs(cosThetaY) > 1e-6) {
    thetaX = std::atan2(Rzy, Rzz);  // Rotation around X-axis
    thetaZ = std::atan2(Ryx, Rxx);  // Rotation around Z-axis
  } else {
    // Handle the gimbal lock case (when cos(thetaY) is close to zero)
    thetaX = 0;
    thetaZ = std::atan2(-Rxy, Ryy);
  }

  displaced.GetRotationAttr().Set(pxr::GfVec3d(thetaX/M_PI*180, thetaY/M_PI*180, thetaZ/M_PI*180));
  displaced.GetTranslationAttr().Set(pxr::GfVec3d(displaced_translation[0],
                                                  displaced_translation[1],
                                                  displaced_translation[2]));

  displaced.Update();

  return solid->GetName();
}

G4String G4USDWriter::ProcessSubtraction(const pxr::UsdStagePtr &stage, pxr::UsdPrim& prim, G4SubtractionSolid* solid) {
  std::cout << "G4USDWriter::ProcessSubtraction " << solid->GetName() << std::endl;

  auto solid_path = prim.GetPath().AppendPath(pxr::SdfPath(solid->GetName()));
  auto subtraction = pxr::G4Subtraction::Define(stage,solid_path);
  auto subtraction_prim = subtraction.GetPrim();

  std::cout << "G4USDWriter::ProcessSubtraction " <<  solid->GetConstituentSolid(0) << " " << solid->GetConstituentSolid(1) << std::endl;
  auto solid1_name = this->ProcessSolid(stage, subtraction_prim, solid->GetConstituentSolid(0));
  auto solid2_name = this->ProcessSolid(stage, subtraction_prim, solid->GetConstituentSolid(1));
  G4String solid3_name = G4String("result");

  // make result
  auto result_mesh = pxr::UsdGeomMesh::Define(stage,solid_path.AppendPath(pxr::SdfPath(solid3_name)));

  // set names
  subtraction.GetSolid1primAttr().Set(std::string(solid1_name));
  subtraction.GetSolid2primAttr().Set(std::string(solid2_name));
  subtraction.GetSolid3primAttr().Set(std::string(solid3_name));

  // update
  subtraction.Update();

  return subtraction_prim.GetName().GetString();
}

G4String G4USDWriter::ProcessUnion(const pxr::UsdStagePtr& stage, pxr::UsdPrim& prim, G4UnionSolid* solid) {
  std::cout << "G4USDWriter::ProcessUnion " << solid->GetName() << std::endl;

  auto solid_path = prim.GetPath().AppendPath(pxr::SdfPath(solid->GetName()));
  auto union_pxr = pxr::G4Union::Define(stage,solid_path);
  auto union_prim = union_pxr.GetPrim();

  std::cout << "G4USDWriter::ProcessUnion " <<  solid->GetConstituentSolid(0) << " " << solid->GetConstituentSolid(1) << std::endl;
  auto solid1_name = this->ProcessSolid(stage, union_prim, solid->GetConstituentSolid(0));
  auto solid2_name = this->ProcessSolid(stage, union_prim, solid->GetConstituentSolid(1));
  G4String solid3_name = G4String("result");

  // make result
  auto result_mesh = pxr::UsdGeomMesh::Define(stage,solid_path.AppendPath(pxr::SdfPath(solid3_name)));

  // set names
  union_pxr.GetSolid1primAttr().Set(std::string(solid1_name));
  union_pxr.GetSolid2primAttr().Set(std::string(solid2_name));
  union_pxr.GetSolid3primAttr().Set(std::string(solid3_name));

  // update
  union_pxr.Update();

  return union_prim.GetName().GetString();
}

G4String G4USDWriter::ProcessIntersection(const pxr::UsdStagePtr& stage, pxr::UsdPrim& prim, G4IntersectionSolid* solid) {
  std::cout << "G4USDWriter::ProcessIntersection " << solid->GetName() << std::endl;

  auto solid_path = prim.GetPath().AppendPath(pxr::SdfPath(solid->GetName()));
  auto intersection_pxr = pxr::G4Intersection::Define(stage,solid_path);
  auto intersection_prim = intersection_pxr.GetPrim();

  std::cout << "G4USDWriter::ProcessIntersection " <<  solid->GetConstituentSolid(0) << " " << solid->GetConstituentSolid(1) << std::endl;
  auto solid1_name = this->ProcessSolid(stage, intersection_prim, solid->GetConstituentSolid(0));
  auto solid2_name = this->ProcessSolid(stage, intersection_prim, solid->GetConstituentSolid(1));
  G4String solid3_name = G4String("result");

  // make result
  auto result_mesh = pxr::UsdGeomMesh::Define(stage,solid_path.AppendPath(pxr::SdfPath(solid3_name)));

  // set names
  intersection_pxr.GetSolid1primAttr().Set(std::string(solid1_name));
  intersection_pxr.GetSolid2primAttr().Set(std::string(solid2_name));
  intersection_pxr.GetSolid3primAttr().Set(std::string(solid3_name));

  // update
  intersection_pxr.Update();

  return intersection_prim.GetName().GetString();
}

G4String G4USDWriter::ProcessMultiUnion(const pxr::UsdStagePtr& stage, pxr::UsdPrim& prim, G4MultiUnion* solid) {
  std::cout << "G4USDWriter::ProcessMultiUnion " << solid->GetName() << std::endl;

  auto solid_path = prim.GetPath().AppendPath(pxr::SdfPath(solid->GetName()));
  auto multiunion_pxr = pxr::G4MultiUnion::Define(stage,solid_path);
  auto multiunion_prim = multiunion_pxr.GetPrim();

  auto nsolid = solid->GetNumberOfSolids();
  std::cout << "G4USDWriter::ProcessMultiUnion " << nsolid << std::endl;

  pxr::VtArray<std::string> solidprim_names;

  for(G4int j=0; j<nsolid; j++) {
    auto displacedSolid = new G4DisplacedSolid(solid->GetName() + "_displaced_" + std::to_string(j),
                                               solid->GetSolid(j),
                                               solid->GetTransformation(j));
    std::cout << j << " " << solid->GetSolid(j) << " " << solid->GetConstituentSolid(j) << std::endl;
    auto displaced_name = ProcessSolid(stage, multiunion_prim, displacedSolid);
    solidprim_names.push_back(std::string(displaced_name));
  }

  // set names
  G4String solid3_name = G4String("result");
  multiunion_pxr.GetSolid3primAttr().Set(std::string(solid3_name));
  multiunion_pxr.GetSolidprimsAttr().Set(solidprim_names);

  // make result
  auto result_mesh = pxr::UsdGeomMesh::Define(stage,solid_path.AppendPath(pxr::SdfPath(solid3_name)));

  multiunion_pxr.Update();

  return multiunion_prim.GetName().GetString();
}
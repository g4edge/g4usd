#include "G4String.hh"
#include "G4VPhysicalVolume.hh"

#include "G4USDParser.hh"
#include "G4USDReader.hh"
#include "G4USDWriter.hh"

G4USDParser::G4USDParser() {}

G4USDParser::~G4USDParser() {
  delete reader;
}

void G4USDParser::Read(const G4String& filename) {
  if(reader != nullptr) {
    delete reader;
  }
  reader = new G4USDReader(filename);
}

void G4USDParser::Write(const G4String& filename, G4LogicalVolume *lv_top) {
  if(writer != nullptr) {
    delete writer;
  }
  writer = new G4USDWriter(filename, lv_top);
}

void G4USDParser::Write(const G4String& filename, G4VPhysicalVolume *pv_top) {
  if(writer != nullptr) {
    delete writer;
  }
  writer = new G4USDWriter(filename, pv_top->GetLogicalVolume());
}


G4VPhysicalVolume* G4USDParser::GetWorldVolume() const {
  return reader->GetWorldVolume();
}
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

#ifndef G4USDPARSER_HH
#define G4USDPARSER_HH

class G4String;
class G4VPhysicalVolume;
class G4USDReader;
class G4USDWriter;
class G4LogicalVolume;

class G4USDParser {
  public:
    G4USDParser();
    ~G4USDParser();
    void Read(const G4String& filename);
    void Write(const G4String& filename, G4LogicalVolume *lv_top);
    void Write(const G4String& filename, G4VPhysicalVolume *pv_top);

    G4VPhysicalVolume* GetWorldVolume() const;

  private:
    G4USDReader *reader = nullptr;
    G4USDWriter *writer = nullptr;
};

#endif  // G4USDPARSER_HH

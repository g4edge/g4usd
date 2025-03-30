# - G4usd module build definition

# Define the Geant4 Module.
geant4_add_module(G4usd
        PUBLIC_HEADERS
        G4USDParser.hh
        G4USDReader.hh
        G4USDWriter.hh
        SOURCES
        G4USDParser.cc
        G4USDReader.cc
        G4USDWriter.cc
)

geant4_module_include_directories(G4usd
        PRIVATE
        ${PXR_INCLUDE_DIRS})

geant4_module_link_libraries(G4usd
        PUBLIC
        G4globman
        G4geometrymng
        G4materials
        ${PXR_LIBRARIES}
        # TODO find a better discovery of usd plugins
        /Users/stewart.boogert/Dropbox/Physics/coderepos/openusd/openusd-build/plugin/usd/g4.dylib
)
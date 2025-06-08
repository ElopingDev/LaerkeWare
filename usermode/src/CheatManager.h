#pragma once

#include <memory>
#include <vector>
#include <Windows.h> // for DWORD

// Forward declarations for pointers are OK, but unique_ptr needs complete types
#include "memory/MemoryHandler.h"
#include "cheats/OffsetHandler.h"
#include "cheats/ESP.h"
#include "Overlay.h"
#include "genCode.h"

class CheatManager
{
public:
    // Declare constructor taking ownership of MemoryHandler and pid
    CheatManager(std::unique_ptr<MemoryHandler> memHandler, DWORD pid);
    void mainLoop();

private:
    std::unique_ptr<OffsetHandler> offsetHandler;
    std::unique_ptr<ESP> esp;
    std::unique_ptr<Overlay> overlay;

    Player& player;  // references, must bind to valid objects
    std::vector<std::shared_ptr<Entity>>& entities;

    void rereadPointers();
    void render();
};

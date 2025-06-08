#pragma once
#include <cstdint>
#include <Windows.h> // For DWORD
#include <memory>
#include <vector>

class Entity;
class MemoryHandler;
class Player;

class OffsetHandler {
public:
    // Constructor takes ownership of MemoryHandler
    OffsetHandler(std::unique_ptr<MemoryHandler> memHandler, DWORD targetPid);
    ~OffsetHandler();

    void readPointers();
    Player& getLocalPlayer();

    // Return MemoryHandler pointer (const and non-const versions)
    MemoryHandler* getMemoryHandler() { return memoryHandler.get(); }

    std::vector<std::shared_ptr<Entity>>& getEntities();
    float* getViewMatrix() { return viewMatrix; }

private:
    std::unique_ptr<MemoryHandler> memoryHandler;

    std::uint32_t clientBase = 0;
    std::uint32_t serverBase = 0;
    std::uint32_t engineBase = 0;

    std::unique_ptr<Player> player;
    std::vector<std::shared_ptr<Entity>> entities;
    float viewMatrix[16] = {};

    DWORD pid = 0; // store the target PID
};

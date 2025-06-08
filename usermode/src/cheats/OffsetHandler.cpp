#include "OffsetHandler.h"
#include "../memory/MemoryHandler.h"
#include "../genCode.h"
#include "../utils.hpp"
#include <iostream>

#define OFFSET_PLAYER 0x0075E9C8
#define OFFSET_ENTITIES 0x0075E9D8
#define OFFSET_VIEW_MATRIX 0x601FEC


OffsetHandler::OffsetHandler(std::unique_ptr<MemoryHandler> memHandler, DWORD targetPid) {
    memoryHandler = std::move(memHandler);  // move ownership
    pid = targetPid;
    player = std::make_unique<Player>();
    clientBase = memoryHandler->getModuleBaseAddress(pid, L"client.dll");
    serverBase = memoryHandler->getModuleBaseAddress(pid, L"server.dll");
    engineBase = memoryHandler->getModuleBaseAddress(pid, L"engine.dll");
}


OffsetHandler::~OffsetHandler() {
}

void OffsetHandler::readPointers()
{
    uintptr_t playerBase = serverBase + OFFSET_PLAYER;

    // Read the pointer chain using the 32bit function
    std::uintptr_t offsets[] = { 0x0, 0x0 };
    uintptr_t playerAddr = utils::resolve_pointer_chain_32(*memoryHandler, playerBase, offsets, 2);
    *player = memoryHandler->ReadMemory<Player>(playerAddr);

    uint32_t baseEntitiesPtr = memoryHandler->ReadMemory<uint32_t>(serverBase + OFFSET_ENTITIES);

    uint32_t entitiesAddr = baseEntitiesPtr + 0x4;  // add 4 because entities start 4 bytes after base ptr

    memoryHandler->readPointerArray(entitiesAddr, entities, 64, true);

    uint32_t viewMatrixBase = memoryHandler->ReadMemory<uint32_t>(engineBase + OFFSET_VIEW_MATRIX);
    uint32_t viewMatrixAddr = viewMatrixBase + 0x2E4;
    memoryHandler->readArray<float>(viewMatrixAddr, viewMatrix, 16);
}


Player& OffsetHandler::getLocalPlayer() {
    return *player;
}

//MemoryHandler& OffsetHandler::getMemoryHandler() {
//	return *memoryHandler;
//}

std::vector<std::shared_ptr<Entity>>& OffsetHandler::getEntities()
{
    return entities;
}

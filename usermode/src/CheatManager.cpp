#include "CheatManager.h"

#include <Windows.h> // for Sleep

CheatManager::CheatManager(std::unique_ptr<MemoryHandler> memHandler, DWORD pid) :
    offsetHandler(std::make_unique<OffsetHandler>(std::move(memHandler), pid)),
    esp(std::make_unique<ESP>()),
    overlay(std::make_unique<Overlay>(L"LaerkeWare Overlay")),
    player(offsetHandler->getLocalPlayer()),  // getLocalPlayer() returns Player& 
    entities(offsetHandler->getEntities())   // getEntities() returns std::vector<shared_ptr<Entity>>&
{
    int width, height, offsetX, offsetY;
    offsetHandler->getMemoryHandler()->getWindowSize(width, height, offsetX, offsetY);
    esp->setWindowOffsets(offsetX, offsetY);
    esp->setWindowSize(width, height);
}

void CheatManager::mainLoop()
{
    while (true) {
        rereadPointers();
        render();
        Sleep(1 / 120); 
    }
}

void CheatManager::rereadPointers()
{
    offsetHandler->readPointers();
}

void CheatManager::render()
{
    overlay->handleMessages();
    overlay->startRender();

    esp->drawESP(player, entities, offsetHandler->getViewMatrix());  // pass references, not pointers

    overlay->render();
    if (overlay->isMenuVisible())
        esp->drawSettings();

    overlay->endRender();
}

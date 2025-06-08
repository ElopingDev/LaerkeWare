
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "memory/MemoryHandler.h"
#include "utils.hpp"
#include "Overlay.h"
#include "genCode.h"
#include "cheats/OffsetHandler.h"
#include "CheatManager.h"  // include your CheatManager header
#include "cheats/ESP.h"

int main() {
    std::cout << "laerkeware\n";

    DWORD pid = MemoryHandler::getProcessId(L"left4dead2.exe");
    if (pid == 0) {
        std::cout << "failed to find process :(\n";
        std::cin.get();
        return 1;
    }

    HANDLE driverHandle = CreateFile(
        L"\\\\.\\Laerdriver",
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (driverHandle == INVALID_HANDLE_VALUE) {
        std::cout << "failed to create driver handle :(\n";
        std::cin.get();
        return 1;
    }

    auto memHandler = std::make_unique<MemoryHandler>(driverHandle);

    if (!memHandler->AttachToProcess(pid)) {
        std::cout << "failed to attach to process :(\n";
        CloseHandle(driverHandle);
        std::cin.get();
        return 1;
    }

    // Create CheatManager with memHandler and pid (memHandler moved)
    CheatManager cheatManager(std::move(memHandler), pid);

    // Run main loop (will include your overlay render loop)
    cheatManager.mainLoop();

    CloseHandle(driverHandle);
    std::cin.get();
    return 0;
}

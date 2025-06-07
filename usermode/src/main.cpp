#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "headers.hpp"
#include "offsets.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "Overlay.h"

// Helper to read a 32-bit pointer from memory (since game is 32-bit)
uintptr_t read_ptr32(HANDLE driver, uintptr_t address) {
    return static_cast<uintptr_t>(driver::read_memory<uint32_t>(driver, address));
}

static DWORD get_process_id(const wchar_t* process_name) {
    DWORD process_id = 0;
    HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snap_shot == INVALID_HANDLE_VALUE)
        return process_id;

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snap_shot, &entry)) {
        do {
            if (_wcsicmp(process_name, entry.szExeFile) == 0) {
                process_id = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap_shot, &entry));
    }

    CloseHandle(snap_shot);
    return process_id;
}

static uintptr_t get_module_base(DWORD pid, const wchar_t* module_name) {
    uintptr_t module_base = 0;
    HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap_shot == INVALID_HANDLE_VALUE)
        return module_base;

    MODULEENTRY32W entry = {};
    entry.dwSize = sizeof(entry);

    if (Module32FirstW(snap_shot, &entry)) {
        do {
            if (wcsstr(module_name, entry.szModule) != nullptr) {
                module_base = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
                break;
            }
        } while (Module32NextW(snap_shot, &entry));
    }

    CloseHandle(snap_shot);
    return module_base;
}

int main() {
    std::cout << "laerkeware\n";

    DWORD pid = get_process_id(L"left4dead2.exe");
    if (pid == 0) {
        std::cout << "failed to find process :(\n";
        std::cin.get();
        return 1;
    }

    HANDLE driver = CreateFile(L"\\\\.\\Laerdriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (driver == INVALID_HANDLE_VALUE) {
        std::cout << "failed to create driver handle :(\n";
        std::cin.get();
        return 1;
    }

    if (driver::attach_to_process(driver, pid)) {
        std::cout << "attached to process!! yay :3\n";

        uintptr_t server = get_module_base(pid, L"server.dll");
        if (server != 0) {
            std::cout << "game found! base address : 0x" << std::hex << server << std::dec << "\n";

            //Overlay overlay(L"LaerkeWare Overlay");
            //	overlay.renderLoop();

            uint32_t entity_list_ptr = driver::read_memory<uint32_t>(driver, (uint32_t)(server + 0x75E9C8));

            std::cout << "Entity list base ptr: 0x" << std::hex << entity_list_ptr << std::dec << "\n";

            int i = 0;
            while (i < 128) {  // or whatever max count you want
                uint32_t entity_ptr = driver::read_memory<uint32_t>(driver, entity_list_ptr + i * 4);
                if (entity_ptr == 0)
                    break;  // no more entities

                if (i == 0)
                    std::cout << "Local player @ 0x" << std::hex << entity_ptr << std::dec << "\n";
                else
                    std::cout << "Entity #" << i << " @ 0x" << std::hex << entity_ptr << std::dec << "\n";

                int health = driver::read_memory<int>(driver, entity_ptr + 0xEC);
                std::cout << "  Health: " << health << "\n";

                i++;
            }





        }
        else {
            std::cout << "Failed to find server.dll\n";
        }
    }

    CloseHandle(driver);
    std::cin.get();
    return 0;
}

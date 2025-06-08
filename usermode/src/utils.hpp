#pragma once
#include "memory/MemoryHandler.h"
#include <cstddef>
#include <cstdint>
#include <iostream>

namespace utils {

    inline std::uintptr_t resolve_pointer_chain_32(MemoryHandler& memHandler, std::uintptr_t base, const std::uintptr_t* offsets, size_t count) {
        std::uintptr_t address = memHandler.ReadMemory<std::uint32_t>(base);

        for (size_t i = 0; i < count - 1; ++i) {
            address += offsets[i];
            address = memHandler.ReadMemory<std::uint32_t>(address);
        }

        address += offsets[count - 1];
        return address;
    }

    inline std::uintptr_t resolve_pointer_chain_64(MemoryHandler& memHandler, std::uintptr_t base, const std::uintptr_t* offsets, size_t count) {
        std::uintptr_t address = memHandler.ReadMemory<std::uintptr_t>(base);

        for (size_t i = 0; i < count - 1; ++i) {
            address += offsets[i];
            address = memHandler.ReadMemory<std::uintptr_t>(address);
        }

        address += offsets[count - 1];
        return address;
    }

}

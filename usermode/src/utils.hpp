#pragma once
#include "headers.hpp"
#include "offsets.hpp"

namespace utils {

    std::uintptr_t resolve_pointer_chain_32(HANDLE driver, std::uintptr_t base, const std::uintptr_t* offsets, size_t count) {
        std::uintptr_t address = static_cast<std::uintptr_t>(driver::read_memory<std::uint32_t>(driver, base));

        for (size_t i = 0; i < count - 1; ++i) {
            address += offsets[i];
            address = static_cast<std::uintptr_t>(driver::read_memory<std::uint32_t>(driver, address));
        }

        address += offsets[count - 1];
        return address;
    }

    std::uintptr_t resolve_pointer_chain_64(HANDLE driver, std::uintptr_t base, const std::uintptr_t* offsets, size_t count) {
        std::uintptr_t address = driver::read_memory<std::uintptr_t>(driver, base);

        for (size_t i = 0; i < count - 1; ++i) {
            address += offsets[i];
            address = driver::read_memory<std::uintptr_t>(driver, address);
        }

        address += offsets[count - 1];
        return address;
    }
}

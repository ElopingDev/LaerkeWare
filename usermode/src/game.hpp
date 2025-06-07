#pragma once
#include "headers.hpp"
#include "offsets.hpp"
#include "utils.hpp"

namespace game {
    inline std::uintptr_t get_local_player(std::uintptr_t base) {
        return base + offsets::LOCALPLAYER;
    }
	// =========== Ammo ===========
    inline std::uintptr_t get_ammo_address(HANDLE driver, std::uintptr_t base) {
        const std::uintptr_t local_player = get_local_player(base);
        return utils::resolve_pointer_chain_32(driver, local_player, offsets::AMMO_OFFSETS, std::size(offsets::AMMO_OFFSETS));
    }

    inline DWORD get_player_ammo(HANDLE driver, std::uintptr_t base) {
        return driver::read_memory<DWORD>(driver, get_ammo_address(driver, base));
    }

    inline void set_player_ammo(HANDLE driver, std::uintptr_t base, DWORD value) {
        driver::write_memory<DWORD>(driver, get_ammo_address(driver, base), value);
    }
    // =========== Ammo ===========

}

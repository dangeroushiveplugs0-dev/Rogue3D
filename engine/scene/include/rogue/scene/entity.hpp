#pragma once

#include <cstdint>
#include "rogue/core/types.hpp"

namespace rogue::scene {

struct Entity {
    rogue::core::EntityId id = 0;
    bool active = true;

    constexpr explicit operator bool() const noexcept { return id != 0; }
};

}
#pragma once

#include <cstddef>
#include <unordered_map>
#include "rogue/scene/entity.hpp"
#include "rogue/scene/transform.hpp"

namespace rogue::scene {

class Scene {
public:
    Entity create_entity();
    bool destroy_entity(Entity entity);
    bool is_alive(Entity entity) const noexcept;

    Transform* transform(Entity entity) noexcept;
    const Transform* transform(Entity entity) const noexcept;
    std::size_t entity_count() const noexcept { return transforms_.size(); }

private:
    rogue::core::EntityId next_id_ = 1;
    std::unordered_map<rogue::core::EntityId, Transform> transforms_;
};

}
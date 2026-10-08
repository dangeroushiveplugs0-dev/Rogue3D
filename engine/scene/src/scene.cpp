#include "rogue/scene/scene.hpp"

namespace rogue::scene {

Entity Scene::create_entity() {
    const auto id = next_id_++;
    transforms_.emplace(id, Transform{});
    return Entity{id, true};
}

bool Scene::destroy_entity(Entity entity) {
    if (!entity) return false;
    return transforms_.erase(entity.id) != 0;
}

bool Scene::is_alive(Entity entity) const noexcept {
    return entity && transforms_.find(entity.id) != transforms_.end();
}

Transform* Scene::transform(Entity entity) noexcept {
    if (!is_alive(entity)) return nullptr;
    return &transforms_.at(entity.id);
}

const Transform* Scene::transform(Entity entity) const noexcept {
    if (!is_alive(entity)) return nullptr;
    return &transforms_.at(entity.id);
}

}
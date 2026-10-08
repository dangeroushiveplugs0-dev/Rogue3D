#include <cassert>
#include "rogue/scene/scene.hpp"

int main() {
    rogue::scene::Scene scene;
    assert(scene.entity_count() == 0);

    const auto entity = scene.create_entity();
    assert(entity);
    assert(scene.is_alive(entity));
    assert(scene.entity_count() == 1);

    auto* transform = scene.transform(entity);
    assert(transform != nullptr);
    transform->position.x = 2.0f;
    transform->scale.z = 3.0f;

    assert(scene.destroy_entity(entity));
    assert(!scene.is_alive(entity));
    assert(scene.entity_count() == 0);
    return 0;
}

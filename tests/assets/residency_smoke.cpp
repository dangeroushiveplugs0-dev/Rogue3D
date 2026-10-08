#include "rogue/assets/asset_residency.hpp"

#include <cassert>

int main() {
    rogue::assets::ResidencyBudget budget;
    budget.total_bytes = 100;
    rogue::assets::AssetResidencyManager manager(budget);

    assert(manager.register_asset({1, rogue::assets::ResidencyClass::Geometry, 60, false}));
    assert(manager.register_asset({2, rogue::assets::ResidencyClass::Texture, 30, false}));
    assert(manager.register_asset({3, rogue::assets::ResidencyClass::Morph, 40, false}));

    assert(manager.make_resident(1));
    assert(manager.make_resident(2));
    assert(manager.stats().resident_bytes == 90);

    assert(manager.make_resident(3));
    auto stats = manager.stats();
    assert(stats.resident_bytes <= 100);
    assert(stats.evicted_count == 1);
    assert(manager.find(1) != nullptr);
    assert(!manager.find(1)->resident);

    assert(manager.set_required(2, true));
    assert(manager.find(2)->required);
    assert(manager.stats().required_bytes == 30);

    assert(manager.make_resident(1));
    assert(manager.stats().resident_bytes <= 100);
    assert(manager.touch(1));
    assert(manager.unregister_asset(3));

    return 0;
}

#include <cassert>
#include "rogue/importer/asset_importer.hpp"
#include "rogue/importer/blend_bridge.hpp"
int main() {
    rogue::importer::BlendBridge bridge;
    assert(bridge.supports_extension("character.blend"));
    assert(!bridge.supports_extension("character.glb"));
    return 0;
}

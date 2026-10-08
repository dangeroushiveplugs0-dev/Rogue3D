#include <cassert>
#include "rogue/importer/asset_importer.hpp"

int main() {
    using rogue::importer::AssetFormat;
    assert(rogue::importer::detect_format("character.glb") == AssetFormat::GLB);
    assert(rogue::importer::detect_format("character.gltf") == AssetFormat::GLTF);
    assert(rogue::importer::detect_format("character.obj") == AssetFormat::OBJ);
    assert(rogue::importer::detect_format("character.fbx") == AssetFormat::Unknown);
    return 0;
}

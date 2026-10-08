#pragma once
#include <string_view>
namespace rogue::assets {
enum class AssetFormat { Unknown, GLB, GLTF, OBJ, BLEND, RB };
constexpr std::string_view extension(AssetFormat format) noexcept {
    switch (format) {
        case AssetFormat::GLB: return ".glb";
        case AssetFormat::GLTF: return ".gltf";
        case AssetFormat::OBJ: return ".obj";
        case AssetFormat::BLEND: return ".blend";
        case AssetFormat::RB: return ".rb";
        default: return "";
    }
}
}
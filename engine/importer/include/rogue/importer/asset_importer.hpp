#pragma once

#include <string_view>
#include "rogue/mesh/mesh.hpp"
#include "rogue/materials/material.hpp"

namespace rogue::importer {

enum class AssetFormat { Unknown, GLTF, GLB, OBJ };

constexpr AssetFormat detect_format(std::string_view path) noexcept {
    const auto dot = path.find_last_of('.');
    if (dot == std::string_view::npos) return AssetFormat::Unknown;
    const auto ext = path.substr(dot + 1);
    if (ext == "gltf") return AssetFormat::GLTF;
    if (ext == "glb") return AssetFormat::GLB;
    if (ext == "obj") return AssetFormat::OBJ;
    return AssetFormat::Unknown;
}

struct ImportedAsset {
    rogue::mesh::Mesh mesh{};
    rogue::materials::Material material{};
};

class AssetImporter {
public:
    virtual ~AssetImporter() = default;
    virtual bool can_import(AssetFormat format) const noexcept = 0;
    virtual bool import_text(std::string_view data, ImportedAsset& output) const = 0;
};

}
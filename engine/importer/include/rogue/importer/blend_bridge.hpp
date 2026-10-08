#pragma once
#include <string_view>
#include "rogue/assets/asset_document.hpp"
namespace rogue::importer {
class BlendBridge {
public:
    bool supports_extension(std::string_view path) const noexcept;
    bool can_import_with_external_blender() const noexcept;
    bool import_via_bridge(std::string_view blend_path,
                           rogue::assets::AssetDocument& output) const;
};
}
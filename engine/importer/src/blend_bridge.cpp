#include "rogue/importer/blend_bridge.hpp"
namespace rogue::importer {
bool BlendBridge::supports_extension(std::string_view path) const noexcept {
    const auto dot = path.find_last_of('.');
    return dot != std::string_view::npos && path.substr(dot + 1) == "blend";
}
bool BlendBridge::can_import_with_external_blender() const noexcept { return false; }
bool BlendBridge::import_via_bridge(std::string_view blend_path,
                                    rogue::assets::AssetDocument& output) const {
    (void)blend_path; (void)output;
    return false;
}
}
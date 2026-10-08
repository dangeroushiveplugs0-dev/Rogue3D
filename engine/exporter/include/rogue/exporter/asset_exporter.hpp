#pragma once
#include <string_view>
#include "rogue/assets/asset_document.hpp"
#include "rogue/assets/asset_format.hpp"
namespace rogue::exporter {
struct ExportOptions {
    bool embed_textures = true;
    bool preserve_morphs = true;
    bool preserve_animation = true;
};
class AssetExporter {
public:
    virtual ~AssetExporter() = default;
    virtual bool can_export(rogue::assets::AssetFormat format) const noexcept = 0;
    virtual bool export_asset(const rogue::assets::AssetDocument& asset,
                              std::string_view output_path,
                              const ExportOptions& options) const = 0;
};
}
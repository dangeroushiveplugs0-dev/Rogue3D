#pragma once
#include "rogue/exporter/asset_exporter.hpp"
namespace rogue::exporter {
class RbExporter final : public AssetExporter {
public:
    bool can_export(rogue::assets::AssetFormat format) const noexcept override;
    bool export_asset(const rogue::assets::AssetDocument& asset,
                      std::string_view output_path,
                      const ExportOptions& options) const override;
};
}
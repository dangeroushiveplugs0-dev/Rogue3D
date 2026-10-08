#include "rogue/exporter/rb_exporter.hpp"
#include "rogue/assets/rb_serialization.hpp"
#include <fstream>
#include <string>

namespace rogue::exporter {
bool RbExporter::can_export(rogue::assets::AssetFormat format) const noexcept {
    return format == rogue::assets::AssetFormat::RB;
}
bool RbExporter::export_asset(const rogue::assets::AssetDocument& asset,
                              std::string_view output_path,
                              const ExportOptions& options) const {
    (void)options;
    std::vector<std::uint8_t> bytes;
    if (!rogue::assets::serialize_asset_document(asset, bytes)) return false;
    std::ofstream file(std::string(output_path), std::ios::binary);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return static_cast<bool>(file);
}
}

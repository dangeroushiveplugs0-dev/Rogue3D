#include "rogue/exporter/rb_exporter.hpp"
namespace rogue::exporter {
bool RbExporter::can_export(rogue::assets::AssetFormat format) const noexcept {
    return format == rogue::assets::AssetFormat::RB;
}
bool RbExporter::export_asset(const rogue::assets::AssetDocument& asset,
                              std::string_view output_path,
                              const ExportOptions& options) const {
    (void)asset; (void)output_path; (void)options;
    return false;
}
}
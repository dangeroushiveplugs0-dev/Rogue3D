#include "rogue/exporter/rb_exporter.hpp"
#include "rogue/assets/rb_container.hpp"
#include <fstream>
#include <string>
namespace rogue::exporter {
namespace {
std::vector<std::uint8_t> manifest_for(const rogue::assets::AssetDocument& asset) {
 const std::string text=std::string("{\"format\":\"rogueengine.rb\",\"version\":1,\"name\":\"")+asset.name+"\"}";
 return std::vector<std::uint8_t>(text.begin(),text.end());
}
}
bool RbExporter::can_export(rogue::assets::AssetFormat format) const noexcept { return format==rogue::assets::AssetFormat::RB; }
bool RbExporter::export_asset(const rogue::assets::AssetDocument& asset,std::string_view output_path,const ExportOptions& options) const {
 (void)options;
 rogue::assets::RbContainer c;
 if(!c.add_chunk(rogue::assets::rb_chunk::Manifest,manifest_for(asset))) return false;
 std::vector<std::uint8_t> bytes;
 if(!c.serialize(bytes)) return false;
 std::ofstream f(std::string(output_path),std::ios::binary);
 if(!f) return false;
 f.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
 return static_cast<bool>(f);
}
}

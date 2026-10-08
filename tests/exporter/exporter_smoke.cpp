#include <cassert>
#include <fstream>
#include <string>
#include "rogue/assets/rb_container.hpp"
#include "rogue/exporter/rb_exporter.hpp"
int main(){
 rogue::assets::AssetDocument asset; asset.name="smoke";
 rogue::exporter::RbExporter exporter;
 assert(exporter.can_export(rogue::assets::AssetFormat::RB));
 const std::string path="rogue_smoke.rb";
 assert(exporter.export_asset(asset,path,{}));
 std::ifstream file(path,std::ios::binary);
 std::string bytes((std::istreambuf_iterator<char>(file)),{});
 rogue::assets::RbContainer c;
 assert(c.deserialize(bytes));
 assert(c.chunks().size()==1);
 assert(c.chunks()[0].type==rogue::assets::rb_chunk::Manifest);
 return 0;
}

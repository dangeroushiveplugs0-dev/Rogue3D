#include <cassert>
#include "rogue/exporter/rb_exporter.hpp"
#include "rogue/assets/asset_format.hpp"
int main() {
    rogue::exporter::RbExporter exporter;
    assert(exporter.can_export(rogue::assets::AssetFormat::RB));
    assert(!exporter.can_export(rogue::assets::AssetFormat::OBJ));
    assert(rogue::assets::extension(rogue::assets::AssetFormat::RB) == ".rb");
    return 0;
}

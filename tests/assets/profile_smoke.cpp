#include "rogue/assets/asset_profile.hpp"

#include <cassert>
#include <string>

int main() {
    using namespace rogue::assets;

    auto mobile = make_mobile_profile();
    assert(mobile.kind == AssetProfileKind::Mobile);
    assert(mobile.streaming_enabled);
    assert(mobile.prefer_sparse_morphs);
    assert(!mobile.keep_all_morphs);

    AssetDocument document;
    MorphChannel face;
    face.name = "Face";
    ShapeKey smile;
    smile.name = "Smile";
    smile.storage = MorphStorage::Sparse;
    smile.sparse_deltas.push_back({12, 0.1f, 0.0f, 0.0f});

    ShapeKey unused;
    unused.name = "Unused";
    unused.storage = MorphStorage::Dense;
    unused.position_deltas.resize(6);

    MorphDriver driver;
    driver.target = "Smile";
    driver.expression = "jaw_open * 0.5";
    face.shape_keys = {smile, unused};
    face.drivers.push_back(driver);
    document.morphs.push_back(face);

    apply_asset_profile(document, mobile, {"Smile"});
    assert(document.morphs.size() == 1);
    assert(document.morphs[0].shape_keys.size() == 1);
    assert(document.morphs[0].shape_keys[0].name == "Smile");

    const auto report = inspect_asset_profile(document, mobile);
    assert(report.retained_morph_count == 1);
    assert(report.retained_morph_bytes > 0);

    return 0;
}

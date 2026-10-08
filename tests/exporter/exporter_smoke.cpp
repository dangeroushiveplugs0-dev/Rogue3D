#include <cassert>
#include <fstream>
#include <string>
#include "rogue/assets/asset_profile.hpp"
#include "rogue/assets/rb_serialization.hpp"
#include "rogue/exporter/rb_exporter.hpp"

int main() {
    rogue::assets::AssetDocument asset;
    asset.name = "roundtrip";
    asset.source_format = "blend";
    asset.source_application = "Blender";

    rogue::assets::SceneNode node;
    node.name = "Character";
    node.mesh = "Body";
    node.transform.position = {1.0f, 2.0f, 3.0f};
    asset.nodes.push_back(node);

    rogue::assets::MorphChannel morph;
    morph.name = "Body";
    morph.category = "Diffeomorphic";
    morph.body_part = "Torso";

    rogue::assets::ShapeKey dense;
    dense.name = "DenseShape";
    dense.storage = rogue::assets::MorphStorage::Dense;
    dense.position_deltas = {0.1f,0.2f,0.3f,0.4f,0.5f,0.6f};
    dense.weight = 0.5f;
    dense.slider_min = -1.0f;
    dense.slider_max = 2.0f;
    dense.category = "Body";
    dense.body_part = "Torso";
    morph.shape_keys.push_back(dense);

    rogue::assets::ShapeKey sparse;
    sparse.name = "SparseJCM";
    sparse.storage = rogue::assets::MorphStorage::Sparse;
    sparse.sparse_deltas.push_back({1402, 0.1f, 0.2f, 0.3f});
    sparse.sparse_deltas.push_back({9120, -0.4f, 0.5f, -0.6f});
    sparse.weight = 0.75f;
    sparse.slider_min = 0.0f;
    sparse.slider_max = 1.0f;
    sparse.category = "JCM";
    sparse.body_part = "Elbow";
    morph.shape_keys.push_back(sparse);

    rogue::assets::MorphDriver driver;
    driver.target = "SparseJCM";
    driver.expression = "Elbow";
    driver.variables.push_back({"Elbow", "pose.bones[\"Elbow\"].value", 1.25f});
    morph.drivers.push_back(driver);
    asset.morphs.push_back(morph);

    rogue::assets::Armature arm;
    arm.name = "Genesis";
    arm.bones.push_back({"hip", "", {}});
    asset.armatures.push_back(arm);
    asset.constraints.push_back({"hip", "COPY_ROTATION", "spine", 0.75f});

    rogue::assets::AnimationClip clip;
    clip.name = "Idle";
    clip.duration_seconds = 2.0;
    clip.channels.push_back({"Character", "position", {{0.0,{0.0f,0.0f,0.0f}},{1.0,{1.0f,0.0f,0.0f}}}});
    asset.animations.push_back(clip);
    asset.embedded_blobs.push_back({"body_texture",{1,2,3,4}});
    asset.native_metadata = {9,8,7};

    rogue::exporter::RbExporter exporter;
    assert(exporter.can_export(rogue::assets::AssetFormat::RB));
    const std::string path = "rogue_smoke.rb";
    assert(exporter.export_asset(asset,path,{}));

    std::ifstream file(path,std::ios::binary);
    std::string bytes((std::istreambuf_iterator<char>(file)),{});
    rogue::assets::AssetDocument roundtrip;
    assert(rogue::assets::deserialize_asset_document(bytes,roundtrip));

    assert(roundtrip.name==asset.name);
    assert(roundtrip.source_format==asset.source_format);
    assert(roundtrip.nodes.size()==1);
    assert(roundtrip.nodes[0].transform.position.x==1.0f);
    assert(roundtrip.morphs.size()==1);
    assert(roundtrip.morphs[0].shape_keys.size()==2);
    assert(roundtrip.morphs[0].shape_keys[0].storage==rogue::assets::MorphStorage::Dense);
    assert(roundtrip.morphs[0].shape_keys[0].position_deltas.size()==6);
    assert(roundtrip.morphs[0].shape_keys[0].position_deltas[3]==0.4f);
    assert(roundtrip.morphs[0].shape_keys[1].storage==rogue::assets::MorphStorage::Sparse);
    assert(roundtrip.morphs[0].shape_keys[1].sparse_deltas.size()==2);
    assert(roundtrip.morphs[0].shape_keys[1].sparse_deltas[0].vertex_index==1402);
    assert(roundtrip.morphs[0].shape_keys[1].sparse_deltas[1].dz==-0.6f);
    assert(roundtrip.morphs[0].drivers[0].variables[0].scale==1.25f);
    assert(roundtrip.armatures.size()==1);
    assert(roundtrip.constraints.size()==1);
    assert(roundtrip.animations.size()==1);
    assert(roundtrip.embedded_blobs.size()==1);
    assert(roundtrip.native_metadata==asset.native_metadata);

    auto mobile = rogue::assets::make_mobile_profile();
    assert(mobile.streaming_enabled);
    assert(mobile.prefer_sparse_morphs);
    rogue::assets::apply_asset_profile(roundtrip, mobile, {"SparseJCM"});
    assert(roundtrip.morphs[0].shape_keys.size()==1);
    assert(roundtrip.morphs[0].shape_keys[0].name=="SparseJCM");
    return 0;
}
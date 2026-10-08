#include <cassert>
#include <fstream>
#include <string>
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
    rogue::assets::ShapeKey key;
    key.name = "BodyShape";
    key.position_deltas = {0.1f,0.2f,0.3f,0.4f,0.5f,0.6f};
    key.weight = 0.5f;
    key.slider_min = -1.0f;
    key.slider_max = 2.0f;
    key.category = "Body";
    key.body_part = "Torso";
    morph.shape_keys.push_back(key);
    rogue::assets::MorphDriver driver;
    driver.target = "BodyShape";
    driver.expression = "Torso";
    driver.variables.push_back({"Torso", "pose.bones[\"Torso\"].value", 1.25f});
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
    assert(roundtrip.morphs[0].shape_keys.size()==1);
    assert(roundtrip.morphs[0].shape_keys[0].slider_min==-1.0f);
    assert(roundtrip.morphs[0].drivers[0].variables[0].scale==1.25f);
    assert(roundtrip.armatures.size()==1);
    assert(roundtrip.constraints.size()==1);
    assert(roundtrip.animations.size()==1);
    assert(roundtrip.embedded_blobs.size()==1);
    assert(roundtrip.native_metadata==asset.native_metadata);
    return 0;
}

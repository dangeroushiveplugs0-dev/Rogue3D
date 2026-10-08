#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace rogue::assets {
struct Blob { std::string name; std::vector<std::uint8_t> data; };
struct ShapeKey { std::string name; std::vector<float> position_deltas; float weight = 0.0f; };
struct MorphChannel { std::string name; std::vector<ShapeKey> shape_keys; };
struct AnimationClip { std::string name; double duration_seconds = 0.0; };
struct AssetDocument {
    std::string name;
    std::string source_format;
    std::vector<Blob> embedded_blobs;
    std::vector<MorphChannel> morphs;
    std::vector<AnimationClip> animations;
    std::vector<std::uint8_t> native_metadata;
};
}
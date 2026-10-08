#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace rogue::assets {

struct Blob { std::string name; std::vector<std::uint8_t> data; };
struct Vec3 { float x=0.0f; float y=0.0f; float z=0.0f; };
struct Quat { float x=0.0f; float y=0.0f; float z=0.0f; float w=1.0f; };
struct Transform { Vec3 position{}; Quat rotation{}; Vec3 scale{1.0f,1.0f,1.0f}; };

enum class MorphStorage : std::uint8_t {
    Dense = 0,
    Sparse = 1
};

enum class MorphPrecision : std::uint8_t {
    Float32 = 0,
    Float16 = 1,
    Auto = 2
};

struct SparseShapeDelta {
    std::uint32_t vertex_index = 0;
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
};

struct SparseShapeDelta16 {
    std::uint32_t vertex_index = 0;
    std::uint16_t dx = 0;
    std::uint16_t dy = 0;
    std::uint16_t dz = 0;
};

struct ShapeKey {
    std::string name;
    MorphStorage storage = MorphStorage::Dense;
    MorphPrecision precision = MorphPrecision::Float32;

    // Dense representation: XYZ triplets, one triplet per mesh vertex.
    std::vector<float> position_deltas;
    std::vector<std::uint16_t> position_deltas_f16;

    // Sparse representation: only vertices affected by this morph.
    std::vector<SparseShapeDelta> sparse_deltas;
    std::vector<SparseShapeDelta16> sparse_deltas_f16;

    float weight=0.0f;
    float slider_min=0.0f;
    float slider_max=1.0f;
    std::string category;
    std::string body_part;
};
struct MorphDriverVariable { std::string name; std::string source; float scale=1.0f; };
struct MorphDriver { std::string target; std::string expression; std::vector<MorphDriverVariable> variables; };
struct MorphChannel { std::string name; std::string category; std::string body_part; std::vector<ShapeKey> shape_keys; std::vector<MorphDriver> drivers; };

struct MeshBinding { std::string mesh_name; std::vector<std::uint32_t> vertex_indices; std::vector<float> weights; };
struct Bone { std::string name; std::string parent; Transform rest_transform{}; };
struct Armature { std::string name; std::vector<Bone> bones; };

struct AnimationKey { double time_seconds=0.0; std::vector<float> values; };
struct AnimationChannel { std::string target; std::string property; std::vector<AnimationKey> keys; };
struct AnimationClip { std::string name; double duration_seconds=0.0; std::vector<AnimationChannel> channels; };
struct SceneNode { std::string name; std::string parent; std::string mesh; std::string armature; Transform transform{}; };
struct Constraint { std::string owner; std::string type; std::string target; float influence=1.0f; };

struct AssetDocument {
    std::string name;
    std::string source_format;
    std::string source_application;
    std::vector<SceneNode> nodes;
    std::vector<Armature> armatures;
    std::vector<MeshBinding> skin_bindings;
    std::vector<MorphChannel> morphs;
    std::vector<AnimationClip> animations;
    std::vector<Constraint> constraints;
    std::vector<Blob> embedded_blobs;
    std::vector<std::uint8_t> native_metadata;
};
}
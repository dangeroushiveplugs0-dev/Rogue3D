#include "rogue/materials/pbr.hpp"
#include <algorithm>

namespace rogue::materials {

namespace {
float clamp01(float value) noexcept { return std::clamp(value, 0.0f, 1.0f); }
}

PbrMaterial PbrMaterialModel::sanitize(PbrMaterial material) noexcept {
    for (float& channel : material.base_color) channel = clamp01(channel);
    material.metallic = clamp01(material.metallic);
    material.roughness = clamp01(material.roughness);
    material.reflectiveness = clamp01(material.reflectiveness);
    material.wetness = clamp01(material.wetness);
    material.organic = clamp01(material.organic);
    material.dryness = clamp01(material.dryness);
    material.emission_strength = std::max(0.0f, material.emission_strength);
    material.normal_strength = std::max(0.0f, material.normal_strength);
    material.ambient_occlusion = clamp01(material.ambient_occlusion);
    return material;
}

}
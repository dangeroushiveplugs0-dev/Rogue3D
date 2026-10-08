#pragma once

namespace rogue::materials {

struct PbrMaterial {
    float base_color[4]{1.0f, 1.0f, 1.0f, 1.0f};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float reflectiveness = 0.0f;
    float wetness = 0.0f;
    float organic = 0.0f;
    float dryness = 0.0f;
    float emission_strength = 0.0f;
    float normal_strength = 1.0f;
    float ambient_occlusion = 1.0f;
};

class PbrMaterialModel {
public:
    static PbrMaterial sanitize(PbrMaterial material) noexcept;
};

}
#pragma once

namespace rogue::materials {

struct PbrMaterial {
    float base_color[4]{1.0f, 1.0f, 1.0f, 1.0f};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float normal_strength = 1.0f;
    float occlusion_strength = 1.0f;
    float emissive_color[3]{0.0f, 0.0f, 0.0f};
    float emissive_strength = 0.0f;

    // RogueEngine extensions layered on top of standard PBR.
    float reflectiveness = 0.0f;
    float wetness = 0.0f;
    float organic = 0.0f;
    float dryness = 0.0f;
};

}
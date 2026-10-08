#include <cassert>
#include "rogue/materials/material.hpp"

int main() {
    rogue::materials::Material material;
    material.pbr.metallic = 0.8f;
    material.pbr.roughness = 0.2f;
    material.pbr.reflectiveness = 0.9f;
    material.pbr.wetness = 0.7f;
    material.pbr.organic = 0.5f;
    material.pbr.dryness = 0.1f;
    assert(material.pbr.metallic >= 0.0f && material.pbr.metallic <= 1.0f);
    assert(material.pbr.roughness >= 0.0f && material.pbr.roughness <= 1.0f);
    assert(material.pbr.reflectiveness >= 0.0f && material.pbr.reflectiveness <= 1.0f);
    assert(material.pbr.wetness >= 0.0f && material.pbr.wetness <= 1.0f);
    return 0;
}

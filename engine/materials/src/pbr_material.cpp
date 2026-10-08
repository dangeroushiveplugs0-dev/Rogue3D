#include "rogue/materials/pbr_material.hpp"

namespace rogue::materials {

// The material is currently data-only. Shading calculations belong to the
// renderer/backend layer so the same material can target Vulkan, WebGPU,
// OpenGL ES, Metal, or Direct3D 12.
static_assert(sizeof(PbrMaterial) > 0);

}
#pragma once
#include "rogue/rendering/morph_buffer.hpp"
#include <cstdint>
#include <vector>
namespace rogue::rendering::vulkan {
struct PackedGpuMorphData { std::vector<std::uint8_t> bytes; std::vector<std::uint32_t> offsets; };
bool pack_morph_buffer_for_gpu(const MorphBuffer& source,PackedGpuMorphData& output) noexcept;
}
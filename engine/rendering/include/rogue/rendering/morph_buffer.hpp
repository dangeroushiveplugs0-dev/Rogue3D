#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rogue::rendering {

enum class MorphBufferPrecision : std::uint8_t {
    Float32 = 0,
    Float16 = 1
};

struct MorphDelta {
    std::uint32_t vertex_index = 0;
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
};

struct PackedMorphDelta {
    std::uint32_t vertex_index = 0;
    std::uint16_t dx = 0;
    std::uint16_t dy = 0;
    std::uint16_t dz = 0;
};

struct ActiveMorph {
    std::uint32_t morph_id = 0;
    float weight = 0.0f;
};

struct MorphBufferDesc {
    std::uint32_t vertex_count = 0;
    MorphBufferPrecision precision = MorphBufferPrecision::Float32;
};

class MorphBuffer {
public:
    bool initialize(const MorphBufferDesc& desc);
    void clear() noexcept;

    bool set_dense_morph(std::uint32_t morph_id,
                         const std::vector<float>& xyz_deltas);
    bool set_sparse_morph(std::uint32_t morph_id,
                          const std::vector<MorphDelta>& deltas);

    bool set_active_morphs(std::vector<ActiveMorph> active);
    void clear_active_morphs() noexcept;

    std::size_t morph_count() const noexcept;
    std::size_t packed_byte_size() const noexcept;
    const MorphBufferDesc& desc() const noexcept { return desc_; }
    const std::vector<ActiveMorph>& active_morphs() const noexcept { return active_; }

    // Backend-neutral packed data. A graphics backend owns the actual GPU
    // resource and can upload this byte stream to a storage buffer/texture.
    const std::vector<std::uint8_t>& packed_data() const noexcept { return packed_; }

private:
    struct Range {
        std::size_t offset = 0;
        std::size_t bytes = 0;
    };

    MorphBufferDesc desc_{};
    std::vector<Range> ranges_;
    std::vector<ActiveMorph> active_;
    std::vector<std::uint8_t> packed_;
};

} // namespace rogue::rendering

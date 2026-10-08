#include "rogue/rendering/morph_buffer.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace rogue::rendering {
namespace {

std::uint16_t to_half(float value) noexcept {
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    const std::uint32_t sign = (bits >> 16) & 0x8000u;
    const std::uint32_t exp = (bits >> 23) & 0xffu;
    const std::uint32_t mant = bits & 0x7fffffu;
    if (exp == 0xffu)
        return static_cast<std::uint16_t>(sign | (mant ? 0x7e00u : 0x7c00u));
    const int e = static_cast<int>(exp) - 127;
    if (e > 15) return static_cast<std::uint16_t>(sign | 0x7c00u);
    if (e >= -14) {
        std::uint32_t hm = (mant + 0x1000u) >> 13;
        int he = e + 15;
        if (hm == 0x400u) { hm = 0; ++he; }
        if (he >= 31) return static_cast<std::uint16_t>(sign | 0x7c00u);
        return static_cast<std::uint16_t>(sign | (static_cast<std::uint32_t>(he) << 10) | hm);
    }
    if (e >= -24) {
        const int shift = -e - 14;
        const std::uint32_t m = 0x800000u | mant;
        return static_cast<std::uint16_t>(sign | ((m + (1u << (shift + 12))) >> (shift + 13)));
    }
    return static_cast<std::uint16_t>(sign);
}

template<class T>
void append_bytes(std::vector<std::uint8_t>& dst, const T& value) {
    const auto* p = reinterpret_cast<const std::uint8_t*>(&value);
    dst.insert(dst.end(), p, p + sizeof(T));
}

bool append_u16(std::vector<std::uint8_t>& dst, std::uint16_t value) {
    dst.push_back(static_cast<std::uint8_t>(value));
    dst.push_back(static_cast<std::uint8_t>(value >> 8));
    return true;
}

bool append_f32(std::vector<std::uint8_t>& dst, float value) {
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    dst.push_back(static_cast<std::uint8_t>(bits));
    dst.push_back(static_cast<std::uint8_t>(bits >> 8));
    dst.push_back(static_cast<std::uint8_t>(bits >> 16));
    dst.push_back(static_cast<std::uint8_t>(bits >> 24));
    return true;
}

} // namespace

bool MorphBuffer::initialize(const MorphBufferDesc& desc) {
    if (desc.vertex_count == 0) return false;
    clear();
    desc_ = desc;
    return true;
}

void MorphBuffer::clear() noexcept {
    slices_.clear();
    active_.clear();
    packed_.clear();
}

bool MorphBuffer::set_dense_morph(std::uint32_t morph_id,
                                  const std::vector<float>& xyz_deltas) {
    if (desc_.vertex_count == 0 || xyz_deltas.size() != static_cast<std::size_t>(desc_.vertex_count) * 3)
        return false;

    auto it = std::find_if(slices_.begin(), slices_.end(),
                           [&](const MorphSlice& s) { return s.morph_id == morph_id; });
    if (it != slices_.end()) return false;

    MorphSlice slice;
    slice.morph_id = morph_id;
    slice.storage = MorphStorageMode::Dense;
    slice.precision = desc_.precision;
    slice.offset = packed_.size();
    slice.element_count = desc_.vertex_count;

    if (desc_.precision == MorphBufferPrecision::Float32) {
        packed_.reserve(packed_.size() + xyz_deltas.size() * sizeof(float));
        for (float v : xyz_deltas) append_f32(packed_, v);
    } else {
        packed_.reserve(packed_.size() + xyz_deltas.size() * sizeof(std::uint16_t));
        for (float v : xyz_deltas) append_u16(packed_, to_half(v));
    }
    slice.bytes = packed_.size() - slice.offset;
    slices_.push_back(slice);
    return true;
}

bool MorphBuffer::set_sparse_morph(std::uint32_t morph_id,
                                   const std::vector<MorphDelta>& deltas) {
    if (desc_.vertex_count == 0 || deltas.size() > std::numeric_limits<std::uint32_t>::max())
        return false;
    for (const auto& d : deltas)
        if (d.vertex_index >= desc_.vertex_count) return false;

    auto it = std::find_if(slices_.begin(), slices_.end(),
                           [&](const MorphSlice& s) { return s.morph_id == morph_id; });
    if (it != slices_.end()) return false;

    MorphSlice slice;
    slice.morph_id = morph_id;
    slice.storage = MorphStorageMode::Sparse;
    slice.precision = desc_.precision;
    slice.offset = packed_.size();
    slice.element_count = static_cast<std::uint32_t>(deltas.size());

    for (const auto& d : deltas) {
        append_f32(packed_, static_cast<float>(d.vertex_index));
        if (desc_.precision == MorphBufferPrecision::Float32) {
            append_f32(packed_, d.dx); append_f32(packed_, d.dy); append_f32(packed_, d.dz);
        } else {
            append_u16(packed_, to_half(d.dx)); append_u16(packed_, to_half(d.dy)); append_u16(packed_, to_half(d.dz));
        }
    }
    slice.bytes = packed_.size() - slice.offset;
    slices_.push_back(slice);
    return true;
}

bool MorphBuffer::set_active_morphs(std::vector<ActiveMorph> active) {
    for (const auto& m : active) {
        if (find_morph(m.morph_id) == nullptr) return false;
    }
    std::sort(active.begin(), active.end(),
              [](const ActiveMorph& a, const ActiveMorph& b) { return a.morph_id < b.morph_id; });
    active_ = std::move(active);
    return true;
}

void MorphBuffer::clear_active_morphs() noexcept {
    active_.clear();
}

std::size_t MorphBuffer::morph_count() const noexcept {
    return slices_.size();
}

std::size_t MorphBuffer::packed_byte_size() const noexcept {
    return packed_.size();
}

const MorphSlice* MorphBuffer::find_morph(std::uint32_t morph_id) const noexcept {
    for (const auto& s : slices_)
        if (s.morph_id == morph_id) return &s;
    return nullptr;
}

} // namespace rogue::rendering

#include "rogue/assets/asset_profile.hpp"

#include <algorithm>
#include <limits>
#include <unordered_set>
#include <cstring>

namespace rogue::assets {
namespace {

std::size_t shape_key_bytes(const ShapeKey& key) noexcept {
    constexpr std::size_t float_bytes = sizeof(float);
    std::size_t bytes = 0;
    if (key.storage == MorphStorage::Dense) {
        if (key.position_deltas.size() <= std::numeric_limits<std::size_t>::max() / float_bytes)
            bytes += key.position_deltas.size() * float_bytes;
    } else {
        constexpr std::size_t delta_bytes = sizeof(std::uint32_t) + sizeof(float) * 3;
        if (key.sparse_deltas.size() <= std::numeric_limits<std::size_t>::max() / delta_bytes)
            bytes += key.sparse_deltas.size() * delta_bytes;
    }
    return bytes;
}

std::uint16_t float_to_half(float value) noexcept {
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    const std::uint32_t sign = (bits >> 16) & 0x8000u;
    const std::uint32_t exp = (bits >> 23) & 0xffu;
    const std::uint32_t mant = bits & 0x7fffffu;
    if (exp == 0xffu) {
        if (mant == 0) return static_cast<std::uint16_t>(sign | 0x7c00u);
        return static_cast<std::uint16_t>(sign | 0x7e00u);
    }
    int e = static_cast<int>(exp) - 127;
    if (e > 15) return static_cast<std::uint16_t>(sign | 0x7c00u);
    if (e >= -14) {
        const std::uint32_t rounded = mant + 0x1000u;
        std::uint32_t hmant = rounded >> 13;
        int hexp = e + 15;
        if (hmant == 0x400u) { hmant = 0; ++hexp; }
        if (hexp >= 31) return static_cast<std::uint16_t>(sign | 0x7c00u);
        return static_cast<std::uint16_t>(sign | (static_cast<std::uint32_t>(hexp) << 10) | hmant);
    }
    if (e >= -24) {
        const int shift = -e - 14;
        const std::uint32_t mantissa = 0x800000u | mant;
        const std::uint32_t hmant = (mantissa + (1u << (shift + 12))) >> (shift + 13);
        return static_cast<std::uint16_t>(sign | hmant);
    }
    return static_cast<std::uint16_t>(sign);
}

float half_to_float(std::uint16_t h) noexcept {
    const std::uint32_t sign = static_cast<std::uint32_t>(h & 0x8000u) << 16;
    const std::uint32_t exp = (h >> 10) & 0x1fu;
    const std::uint32_t mant = h & 0x3ffu;
    std::uint32_t bits = 0;
    if (exp == 0) {
        if (mant == 0) {
            bits = sign;
        } else {
            std::uint32_t m = mant;
            int e = -14;
            while ((m & 0x400u) == 0) { m <<= 1; --e; }
            m &= 0x3ffu;
            bits = sign | (static_cast<std::uint32_t>(e + 127) << 23) | (m << 13);
        }
    } else if (exp == 0x1fu) {
        bits = sign | 0x7f800000u | (mant << 13);
    } else {
        bits = sign | ((exp - 15u + 127u) << 23) | (mant << 13);
    }
    float value = 0.0f;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

bool apply_morph_precision(AssetDocument& document, MorphPrecision precision) {
    if (precision == MorphPrecision::Float32) {
        for (auto& channel : document.morphs)
            for (auto& key : channel.shape_keys)
                key.precision = MorphPrecision::Float32;
        return true;
    }
    if (precision != MorphPrecision::Float16) return false;
    for (auto& channel : document.morphs) {
        for (auto& key : channel.shape_keys) {
            key.precision = MorphPrecision::Float16;
            if (key.storage == MorphStorage::Dense) {
                key.position_deltas_f16.clear();
                key.position_deltas_f16.reserve(key.position_deltas.size());
                for (float v : key.position_deltas) key.position_deltas_f16.push_back(float_to_half(v));
                key.position_deltas.clear();
            } else {
                key.sparse_deltas_f16.clear();
                key.sparse_deltas_f16.reserve(key.sparse_deltas.size());
                for (const auto& d : key.sparse_deltas)
                    key.sparse_deltas_f16.push_back({d.vertex_index,float_to_half(d.dx),float_to_half(d.dy),float_to_half(d.dz)});
                key.sparse_deltas.clear();
            }
        }
    }
    return true;
}

bool name_required(const ShapeKey& key,
                   const std::unordered_set<std::string>& required) noexcept {
    return required.find(key.name) != required.end();
}

} // namespace

AssetProfile make_full_profile() {
    AssetProfile p;
    p.kind = AssetProfileKind::Full;
    p.max_texture_dimension = 16384;
    p.keep_all_morphs = true;
    p.keep_required_morphs = true;
    p.prefer_sparse_morphs = false;
    p.morph_precision = MorphPrecision::Float32;
    p.streaming_enabled = true;
    return p;
}

AssetProfile make_desktop_profile() {
    AssetProfile p = make_full_profile();
    p.kind = AssetProfileKind::Desktop;
    p.max_texture_dimension = 8192;
    return p;
}

AssetProfile make_mobile_profile() {
    AssetProfile p;
    p.kind = AssetProfileKind::Mobile;
    p.max_texture_dimension = 4096;
    p.max_embedded_texture_bytes = 0;
    p.max_total_resident_bytes = 256u * 1024u * 1024u;
    p.keep_all_morphs = false;
    p.keep_required_morphs = true;
    p.prefer_sparse_morphs = true;
    p.morph_precision = MorphPrecision::Auto;
    p.max_subdivision_level = 2;
    p.streaming_enabled = true;
    return p;
}

bool morph_is_referenced(const AssetDocument& document,
                         const ShapeKey& shape_key) noexcept {
    for (const auto& channel : document.morphs) {
        for (const auto& driver : channel.drivers) {
            if (driver.target == shape_key.name)
                return true;
            for (const auto& variable : driver.variables) {
                if (variable.source == shape_key.name)
                    return true;
            }
        }
    }
    return false;
}

bool apply_asset_profile(AssetDocument& document,
                         const AssetProfile& profile,
                         const std::vector<std::string>& required_morph_names) {
    if (profile.keep_all_morphs)
        return true;

    std::unordered_set<std::string> required(required_morph_names.begin(),
                                             required_morph_names.end());

    for (const auto& channel : document.morphs) {
        for (const auto& key : channel.shape_keys) {
            if (profile.keep_required_morphs && morph_is_referenced(document, key))
                required.insert(key.name);
        }
    }

    for (auto& channel : document.morphs) {
        auto& keys = channel.shape_keys;
        keys.erase(std::remove_if(keys.begin(), keys.end(),
                                  [&](const ShapeKey& key) {
                                      return !name_required(key, required);
                                  }),
                   keys.end());

        if (channel.shape_keys.empty() && channel.drivers.empty()) {
            // Empty channels are harmless, but leaving them in the editable
            // document preserves channel/category structure for future import.
        }
    }

    return true;
}

AssetProfileReport inspect_asset_profile(const AssetDocument& document,
                                         const AssetProfile&) {
    AssetProfileReport report;
    for (const auto& channel : document.morphs) {
        report.original_morph_count += channel.shape_keys.size();
        for (const auto& key : channel.shape_keys) {
            const auto bytes = shape_key_bytes(key);
            report.original_morph_bytes += bytes;
            report.retained_morph_bytes += bytes;
        }
    }

    report.retained_morph_count = report.original_morph_count;

    for (const auto& blob : document.embedded_blobs)
        report.embedded_blob_bytes += blob.data.size();

    report.estimated_resident_bytes =
        report.retained_morph_bytes + report.embedded_blob_bytes;
    return report;
}

} // namespace rogue::assets

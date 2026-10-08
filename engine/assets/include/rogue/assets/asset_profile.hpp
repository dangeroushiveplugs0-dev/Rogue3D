#pragma once

#include "rogue/assets/asset_document.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

namespace rogue::assets {

enum class AssetProfileKind : std::uint8_t {
    Full = 0,
    Desktop = 1,
    Mobile = 2
};

enum class MorphPrecision : std::uint8_t {
    Float32 = 0,
    Float16 = 1,
    Auto = 2
};

struct AssetProfile {
    AssetProfileKind kind = AssetProfileKind::Full;

    // Deployment budgets are advisory limits used by optimization/runtime layers.
    // They never reject a large source .blend or full editable .rb by themselves.
    std::size_t max_texture_dimension = 8192;
    std::size_t max_embedded_texture_bytes = 0;
    std::size_t max_total_resident_bytes = 0;

    // Mobile profiles may keep only explicitly required morphs plus driver targets.
    bool keep_all_morphs = true;
    bool keep_required_morphs = true;
    bool prefer_sparse_morphs = true;
    MorphPrecision morph_precision = MorphPrecision::Auto;

    // 0 means do not impose a subdivision limit at this layer.
    std::uint32_t max_subdivision_level = 0;

    bool streaming_enabled = false;
};

struct AssetProfileReport {
    std::size_t original_morph_count = 0;
    std::size_t retained_morph_count = 0;
    std::size_t original_morph_bytes = 0;
    std::size_t retained_morph_bytes = 0;
    std::size_t embedded_blob_bytes = 0;
    std::size_t estimated_resident_bytes = 0;
};

AssetProfile make_full_profile();
AssetProfile make_desktop_profile();
AssetProfile make_mobile_profile();

bool morph_is_referenced(const AssetDocument& document,
                         const ShapeKey& shape_key) noexcept;

bool apply_asset_profile(AssetDocument& document,
                         const AssetProfile& profile,
                         const std::vector<std::string>& required_morph_names = {});

AssetProfileReport inspect_asset_profile(const AssetDocument& document,
                                         const AssetProfile& profile);

// Portable IEEE-754 binary16 conversion used by the .rb format.
std::uint16_t float_to_half(float value) noexcept;
float half_to_float(std::uint16_t bits) noexcept;

// Convert selected morph payloads to the precision requested by a profile.
bool apply_morph_precision(AssetDocument& document, MorphPrecision precision);

} // namespace rogue::assets

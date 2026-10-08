#include "rogue/assets/asset_profile.hpp"

#include <algorithm>
#include <limits>
#include <unordered_set>

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

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace rogue::assets {

using ResidencyAssetId = std::uint64_t;

enum class ResidencyClass : std::uint8_t {
    Geometry = 0,
    Morph = 1,
    Texture = 2,
    Animation = 3,
    Physics = 4,
    Other = 5
};

struct ResidencyRequest {
    ResidencyAssetId id = 0;
    ResidencyClass type = ResidencyClass::Other;
    std::size_t bytes = 0;
    bool required = false;
};

struct ResidencyEntry {
    ResidencyAssetId id = 0;
    ResidencyClass type = ResidencyClass::Other;
    std::size_t bytes = 0;
    bool required = false;
    std::uint64_t last_used_tick = 0;
    bool resident = false;
};

struct ResidencyBudget {
    std::size_t total_bytes = 256u * 1024u * 1024u;
    std::size_t geometry_bytes = 0;
    std::size_t morph_bytes = 0;
    std::size_t texture_bytes = 0;
    std::size_t animation_bytes = 0;
    std::size_t physics_bytes = 0;
};

struct ResidencyStats {
    std::size_t resident_bytes = 0;
    std::size_t resident_count = 0;
    std::size_t evicted_count = 0;
    std::size_t required_bytes = 0;
};

class AssetResidencyManager {
public:
    explicit AssetResidencyManager(ResidencyBudget budget = {});

    void set_budget(ResidencyBudget budget) noexcept;
    const ResidencyBudget& budget() const noexcept { return budget_; }

    // Registers metadata without allocating the payload itself.
    bool register_asset(const ResidencyRequest& request);
    bool unregister_asset(ResidencyAssetId id);

    // Marks an asset as needed. The caller owns the actual decoded/GPU payload;
    // this manager only tracks residency and memory accounting.
    bool make_resident(ResidencyAssetId id);
    bool make_non_resident(ResidencyAssetId id);

    // Required assets are protected from automatic eviction.
    bool set_required(ResidencyAssetId id, bool required);
    bool touch(ResidencyAssetId id);

    // Evict least-recently-used non-required assets until the requested free
    // space is available or nothing else can be evicted.
    std::size_t trim_to_budget();

    ResidencyStats stats() const noexcept;
    const ResidencyEntry* find(ResidencyAssetId id) const noexcept;

private:
    bool would_exceed_budget(const ResidencyEntry& entry) const noexcept;
    void account_add(const ResidencyEntry& entry) noexcept;
    void account_remove(const ResidencyEntry& entry) noexcept;

    ResidencyBudget budget_{};
    std::unordered_map<ResidencyAssetId, ResidencyEntry> entries_;
    std::size_t resident_bytes_ = 0;
    std::size_t required_bytes_ = 0;
    std::uint64_t tick_ = 0;
    std::size_t evicted_count_ = 0;
};

} // namespace rogue::assets

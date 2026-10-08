#include "rogue/assets/asset_residency.hpp"

#include <algorithm>
#include <limits>

namespace rogue::assets {
namespace {

std::size_t class_budget(const ResidencyBudget& b, ResidencyClass type) noexcept {
    switch (type) {
        case ResidencyClass::Geometry: return b.geometry_bytes;
        case ResidencyClass::Morph: return b.morph_bytes;
        case ResidencyClass::Texture: return b.texture_bytes;
        case ResidencyClass::Animation: return b.animation_bytes;
        case ResidencyClass::Physics: return b.physics_bytes;
        default: return 0;
    }
}

} // namespace

AssetResidencyManager::AssetResidencyManager(ResidencyBudget budget)
    : budget_(budget) {}

void AssetResidencyManager::set_budget(ResidencyBudget budget) noexcept {
    budget_ = budget;
    trim_to_budget();
}

bool AssetResidencyManager::register_asset(const ResidencyRequest& request) {
    if (request.id == 0 || entries_.find(request.id) != entries_.end())
        return false;
    ResidencyEntry entry;
    entry.id = request.id;
    entry.type = request.type;
    entry.bytes = request.bytes;
    entry.required = request.required;
    entry.last_used_tick = ++tick_;
    entries_.emplace(request.id, entry);
    if (request.required) required_bytes_ += request.bytes;
    return true;
}

bool AssetResidencyManager::unregister_asset(ResidencyAssetId id) {
    auto it = entries_.find(id);
    if (it == entries_.end()) return false;
    if (it->second.resident) account_remove(it->second);
    if (it->second.required) required_bytes_ -= it->second.bytes;
    entries_.erase(it);
    return true;
}

bool AssetResidencyManager::would_exceed_budget(const ResidencyEntry& entry) const noexcept {
    if (entry.bytes > budget_.total_bytes ||
        resident_bytes_ > budget_.total_bytes - entry.bytes)
        return true;
    const std::size_t class_limit = class_budget(budget_, entry.type);
    if (class_limit == 0) return false;
    std::size_t class_bytes = 0;
    for (const auto& [id, e] : entries_) {
        if (e.resident && e.type == entry.type) class_bytes += e.bytes;
    }
    return class_bytes > class_limit || entry.bytes > class_limit - class_bytes;
}

void AssetResidencyManager::account_add(const ResidencyEntry& entry) noexcept {
    resident_bytes_ += entry.bytes;
}

void AssetResidencyManager::account_remove(const ResidencyEntry& entry) noexcept {
    resident_bytes_ -= entry.bytes;
}

bool AssetResidencyManager::make_resident(ResidencyAssetId id) {
    auto it = entries_.find(id);
    if (it == entries_.end()) return false;
    auto& entry = it->second;
    entry.last_used_tick = ++tick_;
    if (entry.resident) return true;
    if (would_exceed_budget(entry)) {
        trim_to_budget();
        if (would_exceed_budget(entry)) return false;
    }
    entry.resident = true;
    account_add(entry);
    return true;
}

bool AssetResidencyManager::make_non_resident(ResidencyAssetId id) {
    auto it = entries_.find(id);
    if (it == entries_.end() || !it->second.resident) return false;
    account_remove(it->second);
    it->second.resident = false;
    return true;
}

bool AssetResidencyManager::set_required(ResidencyAssetId id, bool required) {
    auto it = entries_.find(id);
    if (it == entries_.end() || it->second.required == required) return false;
    auto& entry = it->second;
    entry.required = required;
    if (required) required_bytes_ += entry.bytes;
    else required_bytes_ -= entry.bytes;
    return true;
}

bool AssetResidencyManager::touch(ResidencyAssetId id) {
    auto it = entries_.find(id);
    if (it == entries_.end()) return false;
    it->second.last_used_tick = ++tick_;
    return true;
}

std::size_t AssetResidencyManager::trim_to_budget() {
    std::size_t evicted = 0;
    while (resident_bytes_ > budget_.total_bytes) {
        auto victim = entries_.end();
        for (auto it = entries_.begin(); it != entries_.end(); ++it) {
            if (!it->second.resident || it->second.required) continue;
            if (victim == entries_.end() || it->second.last_used_tick < victim->second.last_used_tick)
                victim = it;
        }
        if (victim == entries_.end()) break;
        account_remove(victim->second);
        victim->second.resident = false;
        ++evicted;
        ++evicted_count_;
    }
    return evicted;
}

ResidencyStats AssetResidencyManager::stats() const noexcept {
    ResidencyStats s;
    s.resident_bytes = resident_bytes_;
    s.required_bytes = required_bytes_;
    s.evicted_count = evicted_count_;
    for (const auto& [id, entry] : entries_)
        if (entry.resident) ++s.resident_count;
    return s;
}

const ResidencyEntry* AssetResidencyManager::find(ResidencyAssetId id) const noexcept {
    const auto it = entries_.find(id);
    return it == entries_.end() ? nullptr : &it->second;
}

} // namespace rogue::assets

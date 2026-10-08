#pragma once

#include <string_view>
#include <vector>
#include "rogue/assets/asset_document.hpp"

namespace rogue::assets {
bool serialize_asset_document(const AssetDocument& asset,
                              std::vector<std::uint8_t>& output) noexcept;
bool deserialize_asset_document(std::string_view input,
                                AssetDocument& output) noexcept;
}

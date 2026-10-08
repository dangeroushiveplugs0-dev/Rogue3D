#include "rogue/importer/asset_importer.hpp"

namespace rogue::importer {

// Format-specific decoders will live behind this portable importer contract.
// Keeping the contract small lets us use mature open-source GLTF/OBJ decoders
// later without exposing their types throughout the engine.
static_assert(sizeof(ImportedAsset) > 0);

}
#pragma once
#include <cstdint>
#include <string_view>
#include <vector>
namespace rogue::assets {
struct RbChunk { std::uint32_t type=0; std::vector<std::uint8_t> data; };
class RbContainer {
public:
 static constexpr std::uint32_t kMagic=0x31425252;
 static constexpr std::uint32_t kVersion=1;
 static std::uint32_t fourcc(char a,char b,char c,char d) noexcept;
 bool add_chunk(std::uint32_t type,std::vector<std::uint8_t> data);
 bool serialize(std::vector<std::uint8_t>& output) const;
 bool deserialize(std::string_view input);
 const std::vector<RbChunk>& chunks() const noexcept { return chunks_; }
 void clear() noexcept { chunks_.clear(); }
private: std::vector<RbChunk> chunks_;
};
namespace rb_chunk {
inline constexpr std::uint32_t Manifest=0x464e414d;
inline constexpr std::uint32_t GlbBase=0x42564c47;
inline constexpr std::uint32_t Scene=0x4e435353;
inline constexpr std::uint32_t Morphs=0x4850524d;
inline constexpr std::uint32_t Rigs=0x53474952;
inline constexpr std::uint32_t Animation=0x4d494e41;
inline constexpr std::uint32_t Physics=0x59534850;
inline constexpr std::uint32_t Verse=0x45525356;
inline constexpr std::uint32_t Textures=0x52545854;
inline constexpr std::uint32_t Metadata=0x5441444d;
}
}
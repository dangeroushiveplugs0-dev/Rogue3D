#include "rogue/assets/rb_serialization.hpp"
#include "rogue/assets/rb_container.hpp"

#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace rogue::assets {
namespace {

constexpr std::uint32_t kMaxCount = 100000;
constexpr std::uint32_t kMaxStringBytes = 64u * 1024u * 1024u;

class Writer {
public:
    std::vector<std::uint8_t> data;

    void u8(std::uint8_t value) { data.push_back(value); }

    void u32(std::uint32_t value) {
        data.push_back(static_cast<std::uint8_t>(value));
        data.push_back(static_cast<std::uint8_t>(value >> 8));
        data.push_back(static_cast<std::uint8_t>(value >> 16));
        data.push_back(static_cast<std::uint8_t>(value >> 24));
    }

    void u64(std::uint64_t value) {
        for (unsigned i = 0; i < 8; ++i)
            data.push_back(static_cast<std::uint8_t>(value >> (i * 8)));
    }

    void f32(float value) {
        static_assert(sizeof(float) == sizeof(std::uint32_t));
        static_assert(std::numeric_limits<float>::is_iec559);
        std::uint32_t bits = 0;
        std::memcpy(&bits, &value, sizeof(bits));
        u32(bits);
    }

    void f64(double value) {
        static_assert(sizeof(double) == sizeof(std::uint64_t));
        static_assert(std::numeric_limits<double>::is_iec559);
        std::uint64_t bits = 0;
        std::memcpy(&bits, &value, sizeof(bits));
        u64(bits);
    }

    void str(std::string_view value) {
        if (value.size() > std::numeric_limits<std::uint32_t>::max())
            throw std::length_error("string too large");
        u32(static_cast<std::uint32_t>(value.size()));
        data.insert(data.end(), value.begin(), value.end());
    }

    void u32_vector(const std::vector<std::uint32_t>& values) {
        if (values.size() > kMaxCount) throw std::length_error("vector too large");
        u32(static_cast<std::uint32_t>(values.size()));
        for (auto value : values) u32(value);
    }

    void f32_vector(const std::vector<float>& values) {
        if (values.size() > kMaxCount) throw std::length_error("vector too large");
        u32(static_cast<std::uint32_t>(values.size()));
        for (auto value : values) f32(value);
    }

    void bytes(const std::vector<std::uint8_t>& values) {
        if (values.size() > kMaxCount * 1024u) throw std::length_error("blob too large");
        u32(static_cast<std::uint32_t>(values.size()));
        data.insert(data.end(), values.begin(), values.end());
    }
};

class Reader {
public:
    explicit Reader(std::string_view input) : data(input) {}

    bool u8(std::uint8_t& value) {
        if (remaining() < 1) return false;
        value = static_cast<std::uint8_t>(data[offset++]);
        return true;
    }

    bool u32(std::uint32_t& value) {
        if (remaining() < 4) return false;
        const auto* p = reinterpret_cast<const std::uint8_t*>(data.data() + offset);
        value = static_cast<std::uint32_t>(p[0]) |
                (static_cast<std::uint32_t>(p[1]) << 8) |
                (static_cast<std::uint32_t>(p[2]) << 16) |
                (static_cast<std::uint32_t>(p[3]) << 24);
        offset += 4;
        return true;
    }

    bool u64(std::uint64_t& value) {
        if (remaining() < 8) return false;
        const auto* p = reinterpret_cast<const std::uint8_t*>(data.data() + offset);
        value = 0;
        for (unsigned i = 0; i < 8; ++i)
            value |= static_cast<std::uint64_t>(p[i]) << (i * 8);
        offset += 8;
        return true;
    }

    bool f32(float& value) {
        std::uint32_t bits = 0;
        if (!u32(bits)) return false;
        std::memcpy(&value, &bits, sizeof(value));
        return true;
    }

    bool f64(double& value) {
        std::uint64_t bits = 0;
        if (!u64(bits)) return false;
        std::memcpy(&value, &bits, sizeof(value));
        return true;
    }

    bool str(std::string& value) {
        std::uint32_t size = 0;
        if (!u32(size) || size > kMaxStringBytes || size > remaining()) return false;
        value.assign(data.data() + offset, size);
        offset += size;
        return true;
    }

    bool u32_vector(std::vector<std::uint32_t>& values) {
        std::uint32_t count = 0;
        if (!u32(count) || count > kMaxCount || count > remaining() / 4) return false;
        values.resize(count);
        for (auto& value : values)
            if (!u32(value)) return false;
        return true;
    }

    bool f32_vector(std::vector<float>& values) {
        std::uint32_t count = 0;
        if (!u32(count) || count > kMaxCount || count > remaining() / 4) return false;
        values.resize(count);
        for (auto& value : values)
            if (!f32(value)) return false;
        return true;
    }

    bool bytes(std::vector<std::uint8_t>& values) {
        std::uint32_t count = 0;
        if (!u32(count) || count > kMaxCount * 1024u || count > remaining()) return false;
        values.resize(count);
        if (count) {
            std::memcpy(values.data(), data.data() + offset, count);
            offset += count;
        }
        return true;
    }

    std::size_t remaining() const noexcept { return data.size() - offset; }
    bool done() const noexcept { return offset == data.size(); }

private:
    std::string_view data;
    std::size_t offset = 0;
};

void write_transform(Writer& w, const Transform& t) {
    w.f32(t.position.x); w.f32(t.position.y); w.f32(t.position.z);
    w.f32(t.rotation.x); w.f32(t.rotation.y); w.f32(t.rotation.z); w.f32(t.rotation.w);
    w.f32(t.scale.x); w.f32(t.scale.y); w.f32(t.scale.z);
}

bool read_transform(Reader& r, Transform& t) {
    return r.f32(t.position.x) && r.f32(t.position.y) && r.f32(t.position.z) &&
           r.f32(t.rotation.x) && r.f32(t.rotation.y) && r.f32(t.rotation.z) &&
           r.f32(t.rotation.w) && r.f32(t.scale.x) && r.f32(t.scale.y) && r.f32(t.scale.z);
}

void write_scene(Writer& w, const AssetDocument& a) {
    w.str(a.name); w.str(a.source_format); w.str(a.source_application);
    w.u32(static_cast<std::uint32_t>(a.nodes.size()));
    for (const auto& n : a.nodes) {
        w.str(n.name); w.str(n.parent); w.str(n.mesh); w.str(n.armature);
        write_transform(w, n.transform);
    }
    w.u32(static_cast<std::uint32_t>(a.skin_bindings.size()));
    for (const auto& b : a.skin_bindings) {
        w.str(b.mesh_name); w.u32_vector(b.vertex_indices); w.f32_vector(b.weights);
    }
}

bool read_scene(Reader& r, AssetDocument& a) {
    if (!r.str(a.name) || !r.str(a.source_format) || !r.str(a.source_application)) return false;
    std::uint32_t count = 0;
    if (!r.u32(count) || count > kMaxCount) return false;
    a.nodes.clear(); a.nodes.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        SceneNode n;
        if (!r.str(n.name) || !r.str(n.parent) || !r.str(n.mesh) || !r.str(n.armature) ||
            !read_transform(r, n.transform)) return false;
        a.nodes.push_back(std::move(n));
    }
    if (!r.u32(count) || count > kMaxCount) return false;
    a.skin_bindings.clear(); a.skin_bindings.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        MeshBinding b;
        if (!r.str(b.mesh_name) || !r.u32_vector(b.vertex_indices) || !r.f32_vector(b.weights)) return false;
        if (b.vertex_indices.size() != b.weights.size()) return false;
        a.skin_bindings.push_back(std::move(b));
    }
    return true;
}

void write_rigs(Writer& w, const AssetDocument& a) {
    w.u32(static_cast<std::uint32_t>(a.armatures.size()));
    for (const auto& arm : a.armatures) {
        w.str(arm.name); w.u32(static_cast<std::uint32_t>(arm.bones.size()));
        for (const auto& b : arm.bones) {
            w.str(b.name); w.str(b.parent); write_transform(w, b.rest_transform);
        }
    }
    w.u32(static_cast<std::uint32_t>(a.constraints.size()));
    for (const auto& c : a.constraints) {
        w.str(c.owner); w.str(c.type); w.str(c.target); w.f32(c.influence);
    }
}

bool read_rigs(Reader& r, AssetDocument& a) {
    std::uint32_t count = 0;
    if (!r.u32(count) || count > kMaxCount) return false;
    a.armatures.clear(); a.armatures.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        Armature arm; std::uint32_t bones = 0;
        if (!r.str(arm.name) || !r.u32(bones) || bones > kMaxCount) return false;
        arm.bones.reserve(bones);
        for (std::uint32_t j = 0; j < bones; ++j) {
            Bone b;
            if (!r.str(b.name) || !r.str(b.parent) || !read_transform(r, b.rest_transform)) return false;
            arm.bones.push_back(std::move(b));
        }
        a.armatures.push_back(std::move(arm));
    }
    if (!r.u32(count) || count > kMaxCount) return false;
    a.constraints.clear(); a.constraints.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        Constraint c;
        if (!r.str(c.owner) || !r.str(c.type) || !r.str(c.target) || !r.f32(c.influence)) return false;
        a.constraints.push_back(std::move(c));
    }
    return true;
}

void write_morphs(Writer& w, const AssetDocument& a) {
    w.u32(static_cast<std::uint32_t>(a.morphs.size()));
    for (const auto& m : a.morphs) {
        w.str(m.name); w.str(m.category); w.str(m.body_part);
        w.u32(static_cast<std::uint32_t>(m.shape_keys.size()));
        for (const auto& s : m.shape_keys) {
            w.str(s.name);
            w.u8(static_cast<std::uint8_t>(s.storage));
            if (s.storage == MorphStorage::Dense) {
                if (s.position_deltas.size() % 3 != 0)
                    throw std::invalid_argument("dense shape key deltas must be XYZ triplets");
                w.f32_vector(s.position_deltas);
            } else if (s.storage == MorphStorage::Sparse) {
                if (s.sparse_deltas.size() > kMaxCount)
                    throw std::length_error("sparse delta list too large");
                w.u32(static_cast<std::uint32_t>(s.sparse_deltas.size()));
                for (const auto& d : s.sparse_deltas) {
                    w.u32(d.vertex_index); w.f32(d.dx); w.f32(d.dy); w.f32(d.dz);
                }
            } else {
                throw std::invalid_argument("unknown morph storage");
            }
            w.f32(s.weight); w.f32(s.slider_min); w.f32(s.slider_max);
            w.str(s.category); w.str(s.body_part);
        }
        w.u32(static_cast<std::uint32_t>(m.drivers.size()));
        for (const auto& d : m.drivers) {
            w.str(d.target); w.str(d.expression);
            w.u32(static_cast<std::uint32_t>(d.variables.size()));
            for (const auto& v : d.variables) {
                w.str(v.name); w.str(v.source); w.f32(v.scale);
            }
        }
    }
}

bool read_morphs(Reader& r, AssetDocument& a) {
    std::uint32_t count = 0;
    if (!r.u32(count) || count > kMaxCount) return false;
    a.morphs.clear(); a.morphs.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        MorphChannel m; std::uint32_t n = 0;
        if (!r.str(m.name) || !r.str(m.category) || !r.str(m.body_part) ||
            !r.u32(n) || n > kMaxCount) return false;
        m.shape_keys.reserve(n);
        for (std::uint32_t j = 0; j < n; ++j) {
            ShapeKey s;
            std::uint8_t storage = 0;
            if (!r.str(s.name) || !r.u8(storage)) return false;
            if (storage == static_cast<std::uint8_t>(MorphStorage::Dense)) {
                s.storage = MorphStorage::Dense;
                if (!r.f32_vector(s.position_deltas) || s.position_deltas.size() % 3 != 0) return false;
            } else if (storage == static_cast<std::uint8_t>(MorphStorage::Sparse)) {
                s.storage = MorphStorage::Sparse;
                std::uint32_t delta_count = 0;
                if (!r.u32(delta_count) || delta_count > kMaxCount) return false;
                s.sparse_deltas.resize(delta_count);
                for (auto& d : s.sparse_deltas)
                    if (!r.u32(d.vertex_index) || !r.f32(d.dx) || !r.f32(d.dy) || !r.f32(d.dz)) return false;
            } else {
                return false;
            }
            if (!r.f32(s.weight) || !r.f32(s.slider_min) || !r.f32(s.slider_max) ||
                !r.str(s.category) || !r.str(s.body_part)) return false;
            m.shape_keys.push_back(std::move(s));
        }
        if (!r.u32(n) || n > kMaxCount) return false;
        m.drivers.reserve(n);
        for (std::uint32_t j = 0; j < n; ++j) {
            MorphDriver d; std::uint32_t vars = 0;
            if (!r.str(d.target) || !r.str(d.expression) || !r.u32(vars) || vars > kMaxCount) return false;
            d.variables.reserve(vars);
            for (std::uint32_t k = 0; k < vars; ++k) {
                MorphDriverVariable v;
                if (!r.str(v.name) || !r.str(v.source) || !r.f32(v.scale)) return false;
                d.variables.push_back(std::move(v));
            }
            m.drivers.push_back(std::move(d));
        }
        a.morphs.push_back(std::move(m));
    }
    return true;
}

void write_animation(Writer& w, const AssetDocument& a) {
    w.u32(static_cast<std::uint32_t>(a.animations.size()));
    for (const auto& clip : a.animations) {
        w.str(clip.name); w.f64(clip.duration_seconds);
        w.u32(static_cast<std::uint32_t>(clip.channels.size()));
        for (const auto& ch : clip.channels) {
            w.str(ch.target); w.str(ch.property);
            w.u32(static_cast<std::uint32_t>(ch.keys.size()));
            for (const auto& k : ch.keys) {
                w.f64(k.time_seconds); w.f32_vector(k.values);
            }
        }
    }
}

bool read_animation(Reader& r, AssetDocument& a) {
    std::uint32_t count = 0;
    if (!r.u32(count) || count > kMaxCount) return false;
    a.animations.clear(); a.animations.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        AnimationClip clip; std::uint32_t channels = 0;
        if (!r.str(clip.name) || !r.f64(clip.duration_seconds) ||
            !r.u32(channels) || channels > kMaxCount) return false;
        clip.channels.reserve(channels);
        for (std::uint32_t j = 0; j < channels; ++j) {
            AnimationChannel ch; std::uint32_t keys = 0;
            if (!r.str(ch.target) || !r.str(ch.property) || !r.u32(keys) || keys > kMaxCount) return false;
            ch.keys.reserve(keys);
            for (std::uint32_t k = 0; k < keys; ++k) {
                AnimationKey key;
                if (!r.f64(key.time_seconds) || !r.f32_vector(key.values)) return false;
                ch.keys.push_back(std::move(key));
            }
            clip.channels.push_back(std::move(ch));
        }
        a.animations.push_back(std::move(clip));
    }
    return true;
}

void write_blobs(Writer& w, const std::vector<Blob>& blobs) {
    w.u32(static_cast<std::uint32_t>(blobs.size()));
    for (const auto& b : blobs) { w.str(b.name); w.bytes(b.data); }
}

bool read_blobs(Reader& r, std::vector<Blob>& blobs) {
    std::uint32_t count = 0;
    if (!r.u32(count) || count > kMaxCount) return false;
    blobs.clear(); blobs.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        Blob b;
        if (!r.str(b.name) || !r.bytes(b.data)) return false;
        blobs.push_back(std::move(b));
    }
    return true;
}

std::vector<std::uint8_t> manifest_for(const AssetDocument& asset) {
    std::string text = "{\"format\":\"rogueengine.rb\",\"version\":2,\"name\":\"";
    for (char c : asset.name) {
        if (c == '\\' || c == '\"') { text.push_back('\\'); text.push_back(c); }
        else if (static_cast<unsigned char>(c) < 0x20) text.push_back(' ');
        else text.push_back(c);
    }
    text += "\"}";
    return {text.begin(), text.end()};
}

template<class F>
std::vector<std::uint8_t> make(F&& f) {
    Writer w; f(w); return std::move(w.data);
}

} // namespace

bool serialize_asset_document(const AssetDocument& asset, std::vector<std::uint8_t>& output) noexcept {
    try {
        RbContainer c;
        if (!c.add_chunk(rb_chunk::Manifest, manifest_for(asset))) return false;
        if (!asset.nodes.empty() || !asset.skin_bindings.empty())
            if (!c.add_chunk(rb_chunk::Scene, make([&](Writer& w){ write_scene(w, asset); }))) return false;
        if (!asset.morphs.empty())
            if (!c.add_chunk(rb_chunk::Morphs, make([&](Writer& w){ write_morphs(w, asset); }))) return false;
        if (!asset.armatures.empty() || !asset.constraints.empty())
            if (!c.add_chunk(rb_chunk::Rigs, make([&](Writer& w){ write_rigs(w, asset); }))) return false;
        if (!asset.animations.empty())
            if (!c.add_chunk(rb_chunk::Animation, make([&](Writer& w){ write_animation(w, asset); }))) return false;
        if (!asset.embedded_blobs.empty())
            if (!c.add_chunk(rb_chunk::Textures, make([&](Writer& w){ write_blobs(w, asset.embedded_blobs); }))) return false;
        if (!asset.native_metadata.empty())
            if (!c.add_chunk(rb_chunk::Metadata, asset.native_metadata)) return false;
        return c.serialize(output);
    } catch (...) {
        return false;
    }
}

bool deserialize_asset_document(std::string_view input, AssetDocument& output) noexcept {
    try {
        RbContainer c;
        if (!c.deserialize(input)) return false;
        output = {};
        for (const auto& chunk : c.chunks()) {
            Reader r(std::string_view(reinterpret_cast<const char*>(chunk.data.data()), chunk.data.size()));
            switch (chunk.type) {
                case rb_chunk::Manifest: break;
                case rb_chunk::Scene: if (!read_scene(r, output) || !r.done()) return false; break;
                case rb_chunk::Morphs: if (!read_morphs(r, output) || !r.done()) return false; break;
                case rb_chunk::Rigs: if (!read_rigs(r, output) || !r.done()) return false; break;
                case rb_chunk::Animation: if (!read_animation(r, output) || !r.done()) return false; break;
                case rb_chunk::Textures: if (!read_blobs(r, output.embedded_blobs) || !r.done()) return false; break;
                case rb_chunk::Metadata: output.native_metadata = chunk.data; break;
                default: break;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}
}
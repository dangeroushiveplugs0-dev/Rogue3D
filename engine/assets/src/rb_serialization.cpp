#include "rogue/assets/rb_serialization.hpp"
#include "rogue/assets/rb_container.hpp"
#include <cstring>
#include <string>
#include <type_traits>

namespace rogue::assets {
namespace {
class Writer {
public:
    std::vector<std::uint8_t> data;
    template <typename T> void pod(T value) {
        static_assert(std::is_trivially_copyable_v<T>);
        const auto* p = reinterpret_cast<const std::uint8_t*>(&value);
        data.insert(data.end(), p, p + sizeof(T));
    }
    void str(std::string_view value) {
        pod<std::uint32_t>(static_cast<std::uint32_t>(value.size()));
        data.insert(data.end(), value.begin(), value.end());
    }
    template <typename T> void vector_pod(const std::vector<T>& values) {
        pod<std::uint32_t>(static_cast<std::uint32_t>(values.size()));
        if (!values.empty()) {
            const auto* p = reinterpret_cast<const std::uint8_t*>(values.data());
            data.insert(data.end(), p, p + sizeof(T) * values.size());
        }
    }
};
class Reader {
public:
    explicit Reader(std::string_view input) : data(input) {}
    template <typename T> bool pod(T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        if (remaining() < sizeof(T)) return false;
        std::memcpy(&value, data.data() + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }
    bool str(std::string& value) {
        std::uint32_t size = 0;
        if (!pod(size) || size > remaining()) return false;
        value.assign(data.data() + offset, size);
        offset += size;
        return true;
    }
    template <typename T> bool vector_pod(std::vector<T>& values) {
        std::uint32_t count = 0;
        if (!pod(count) || count > remaining() / sizeof(T)) return false;
        values.resize(count);
        if (count) {
            std::memcpy(values.data(), data.data() + offset, sizeof(T) * count);
            offset += sizeof(T) * count;
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
    w.pod(t.position.x); w.pod(t.position.y); w.pod(t.position.z);
    w.pod(t.rotation.x); w.pod(t.rotation.y); w.pod(t.rotation.z); w.pod(t.rotation.w);
    w.pod(t.scale.x); w.pod(t.scale.y); w.pod(t.scale.z);
}
bool read_transform(Reader& r, Transform& t) {
    return r.pod(t.position.x) && r.pod(t.position.y) && r.pod(t.position.z) &&
           r.pod(t.rotation.x) && r.pod(t.rotation.y) && r.pod(t.rotation.z) &&
           r.pod(t.rotation.w) && r.pod(t.scale.x) && r.pod(t.scale.y) && r.pod(t.scale.z);
}
void write_scene(Writer& w, const AssetDocument& a) {
    w.str(a.name); w.str(a.source_format); w.str(a.source_application);
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(a.nodes.size()));
    for (const auto& n : a.nodes) {
        w.str(n.name); w.str(n.parent); w.str(n.mesh); w.str(n.armature); write_transform(w, n.transform);
    }
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(a.skin_bindings.size()));
    for (const auto& b : a.skin_bindings) {
        w.str(b.mesh_name); w.vector_pod(b.vertex_indices); w.vector_pod(b.weights);
    }
}
bool read_scene(Reader& r, AssetDocument& a) {
    if (!r.str(a.name) || !r.str(a.source_format) || !r.str(a.source_application)) return false;
    std::uint32_t count = 0;
    if (!r.pod(count) || count > 100000) return false;
    a.nodes.clear(); a.nodes.reserve(count);
    for (std::uint32_t i=0;i<count;++i) {
        SceneNode n;
        if (!r.str(n.name) || !r.str(n.parent) || !r.str(n.mesh) || !r.str(n.armature) || !read_transform(r,n.transform)) return false;
        a.nodes.push_back(std::move(n));
    }
    if (!r.pod(count) || count > 100000) return false;
    a.skin_bindings.clear(); a.skin_bindings.reserve(count);
    for (std::uint32_t i=0;i<count;++i) {
        MeshBinding b;
        if (!r.str(b.mesh_name) || !r.vector_pod(b.vertex_indices) || !r.vector_pod(b.weights)) return false;
        if (b.vertex_indices.size() != b.weights.size()) return false;
        a.skin_bindings.push_back(std::move(b));
    }
    return true;
}
void write_rigs(Writer& w, const AssetDocument& a) {
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(a.armatures.size()));
    for (const auto& arm : a.armatures) {
        w.str(arm.name); w.pod<std::uint32_t>(static_cast<std::uint32_t>(arm.bones.size()));
        for (const auto& b : arm.bones) { w.str(b.name); w.str(b.parent); write_transform(w,b.rest_transform); }
    }
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(a.constraints.size()));
    for (const auto& c : a.constraints) { w.str(c.owner); w.str(c.type); w.str(c.target); w.pod(c.influence); }
}
bool read_rigs(Reader& r, AssetDocument& a) {
    std::uint32_t count=0;
    if (!r.pod(count) || count>100000) return false;
    a.armatures.clear(); a.armatures.reserve(count);
    for (std::uint32_t i=0;i<count;++i) {
        Armature arm; std::uint32_t bones=0;
        if (!r.str(arm.name) || !r.pod(bones) || bones>100000) return false;
        arm.bones.reserve(bones);
        for (std::uint32_t j=0;j<bones;++j) {
            Bone b;
            if (!r.str(b.name) || !r.str(b.parent) || !read_transform(r,b.rest_transform)) return false;
            arm.bones.push_back(std::move(b));
        }
        a.armatures.push_back(std::move(arm));
    }
    if (!r.pod(count) || count>100000) return false;
    a.constraints.clear(); a.constraints.reserve(count);
    for (std::uint32_t i=0;i<count;++i) {
        Constraint c;
        if (!r.str(c.owner) || !r.str(c.type) || !r.str(c.target) || !r.pod(c.influence)) return false;
        a.constraints.push_back(std::move(c));
    }
    return true;
}
void write_morphs(Writer& w, const AssetDocument& a) {
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(a.morphs.size()));
    for (const auto& m : a.morphs) {
        w.str(m.name); w.str(m.category); w.str(m.body_part);
        w.pod<std::uint32_t>(static_cast<std::uint32_t>(m.shape_keys.size()));
        for (const auto& s : m.shape_keys) {
            w.str(s.name); w.vector_pod(s.position_deltas);
            w.pod(s.weight); w.pod(s.slider_min); w.pod(s.slider_max);
            w.str(s.category); w.str(s.body_part);
        }
        w.pod<std::uint32_t>(static_cast<std::uint32_t>(m.drivers.size()));
        for (const auto& d : m.drivers) {
            w.str(d.target); w.str(d.expression);
            w.pod<std::uint32_t>(static_cast<std::uint32_t>(d.variables.size()));
            for (const auto& v : d.variables) { w.str(v.name); w.str(v.source); w.pod(v.scale); }
        }
    }
}
bool read_morphs(Reader& r, AssetDocument& a) {
    std::uint32_t count=0;
    if (!r.pod(count) || count>100000) return false;
    a.morphs.clear(); a.morphs.reserve(count);
    for (std::uint32_t i=0;i<count;++i) {
        MorphChannel m; std::uint32_t n=0;
        if (!r.str(m.name)||!r.str(m.category)||!r.str(m.body_part)||!r.pod(n)||n>100000) return false;
        m.shape_keys.reserve(n);
        for (std::uint32_t j=0;j<n;++j) {
            ShapeKey s;
            if (!r.str(s.name)||!r.vector_pod(s.position_deltas)||!r.pod(s.weight)||!r.pod(s.slider_min)||!r.pod(s.slider_max)||!r.str(s.category)||!r.str(s.body_part)) return false;
            m.shape_keys.push_back(std::move(s));
        }
        if (!r.pod(n)||n>100000) return false;
        m.drivers.reserve(n);
        for (std::uint32_t j=0;j<n;++j) {
            MorphDriver d; std::uint32_t vars=0;
            if (!r.str(d.target)||!r.str(d.expression)||!r.pod(vars)||vars>100000) return false;
            d.variables.reserve(vars);
            for (std::uint32_t k=0;k<vars;++k) {
                MorphDriverVariable v;
                if (!r.str(v.name)||!r.str(v.source)||!r.pod(v.scale)) return false;
                d.variables.push_back(std::move(v));
            }
            m.drivers.push_back(std::move(d));
        }
        a.morphs.push_back(std::move(m));
    }
    return true;
}
void write_animation(Writer& w, const AssetDocument& a) {
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(a.animations.size()));
    for (const auto& clip : a.animations) {
        w.str(clip.name); w.pod(clip.duration_seconds);
        w.pod<std::uint32_t>(static_cast<std::uint32_t>(clip.channels.size()));
        for (const auto& ch : clip.channels) {
            w.str(ch.target); w.str(ch.property);
            w.pod<std::uint32_t>(static_cast<std::uint32_t>(ch.keys.size()));
            for (const auto& k : ch.keys) { w.pod(k.time_seconds); w.vector_pod(k.values); }
        }
    }
}
bool read_animation(Reader& r, AssetDocument& a) {
    std::uint32_t count=0;
    if (!r.pod(count)||count>100000) return false;
    a.animations.clear(); a.animations.reserve(count);
    for (std::uint32_t i=0;i<count;++i) {
        AnimationClip clip; std::uint32_t channels=0;
        if (!r.str(clip.name)||!r.pod(clip.duration_seconds)||!r.pod(channels)||channels>100000) return false;
        clip.channels.reserve(channels);
        for (std::uint32_t j=0;j<channels;++j) {
            AnimationChannel ch; std::uint32_t keys=0;
            if (!r.str(ch.target)||!r.str(ch.property)||!r.pod(keys)||keys>100000) return false;
            ch.keys.reserve(keys);
            for (std::uint32_t k=0;k<keys;++k) {
                AnimationKey key;
                if (!r.pod(key.time_seconds)||!r.vector_pod(key.values)) return false;
                ch.keys.push_back(std::move(key));
            }
            clip.channels.push_back(std::move(ch));
        }
        a.animations.push_back(std::move(clip));
    }
    return true;
}
void write_blobs(Writer& w,const std::vector<Blob>& blobs) {
    w.pod<std::uint32_t>(static_cast<std::uint32_t>(blobs.size()));
    for (const auto& b:blobs) { w.str(b.name); w.vector_pod(b.data); }
}
bool read_blobs(Reader& r,std::vector<Blob>& blobs) {
    std::uint32_t count=0;
    if (!r.pod(count)||count>100000) return false;
    blobs.clear(); blobs.reserve(count);
    for (std::uint32_t i=0;i<count;++i) { Blob b; if(!r.str(b.name)||!r.vector_pod(b.data)) return false; blobs.push_back(std::move(b)); }
    return true;
}
std::vector<std::uint8_t> manifest_for(const AssetDocument& asset) {
    std::string text="{\"format\":\"rogueengine.rb\",\"version\":1,\"name\":\"";
    for(char c:asset.name) { if(c=='\\'||c=='\"'){text.push_back('\\');text.push_back(c);} else if(static_cast<unsigned char>(c)<0x20) text.push_back(' '); else text.push_back(c); }
    text+="\"}";
    return {text.begin(),text.end()};
}
template<class F> std::vector<std::uint8_t> make(F&& f) { Writer w; f(w); return std::move(w.data); }
}
bool serialize_asset_document(const AssetDocument& asset,std::vector<std::uint8_t>& output) noexcept {
    try {
        RbContainer c;
        if(!c.add_chunk(rb_chunk::Manifest,manifest_for(asset))) return false;
        if(!asset.nodes.empty()||!asset.skin_bindings.empty()) if(!c.add_chunk(rb_chunk::Scene,make([&](Writer&w){write_scene(w,asset);}))) return false;
        if(!asset.morphs.empty()) if(!c.add_chunk(rb_chunk::Morphs,make([&](Writer&w){write_morphs(w,asset);}))) return false;
        if(!asset.armatures.empty()||!asset.constraints.empty()) if(!c.add_chunk(rb_chunk::Rigs,make([&](Writer&w){write_rigs(w,asset);}))) return false;
        if(!asset.animations.empty()) if(!c.add_chunk(rb_chunk::Animation,make([&](Writer&w){write_animation(w,asset);}))) return false;
        if(!asset.embedded_blobs.empty()) if(!c.add_chunk(rb_chunk::Textures,make([&](Writer&w){write_blobs(w,asset.embedded_blobs);}))) return false;
        if(!asset.native_metadata.empty()) if(!c.add_chunk(rb_chunk::Metadata,asset.native_metadata)) return false;
        return c.serialize(output);
    } catch(...) { return false; }
}
bool deserialize_asset_document(std::string_view input,AssetDocument& output) noexcept {
    try {
        RbContainer c; if(!c.deserialize(input)) return false;
        output={};
        for(const auto& chunk:c.chunks()) {
            Reader r(std::string_view(reinterpret_cast<const char*>(chunk.data.data()),chunk.data.size()));
            switch(chunk.type) {
                case rb_chunk::Manifest: break;
                case rb_chunk::Scene: if(!read_scene(r,output)||!r.done()) return false; break;
                case rb_chunk::Morphs: if(!read_morphs(r,output)||!r.done()) return false; break;
                case rb_chunk::Rigs: if(!read_rigs(r,output)||!r.done()) return false; break;
                case rb_chunk::Animation: if(!read_animation(r,output)||!r.done()) return false; break;
                case rb_chunk::Textures: if(!read_blobs(r,output.embedded_blobs)||!r.done()) return false; break;
                case rb_chunk::Metadata: output.native_metadata=chunk.data; break;
                default: break;
            }
        }
        return true;
    } catch(...) { return false; }
}
}

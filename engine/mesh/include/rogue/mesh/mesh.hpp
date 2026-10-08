#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rogue::mesh {

struct Vertex {
    float position[3]{};
    float normal[3]{0.0f, 0.0f, 1.0f};
    float uv[2]{};
};

using Index = std::uint32_t;

class Mesh {
public:
    bool set_vertices(std::vector<Vertex> vertices);
    bool set_indices(std::vector<Index> indices);

    const std::vector<Vertex>& vertices() const noexcept { return vertices_; }
    const std::vector<Index>& indices() const noexcept { return indices_; }

    std::size_t vertex_count() const noexcept { return vertices_.size(); }
    std::size_t index_count() const noexcept { return indices_.size(); }
    std::size_t triangle_count() const noexcept { return indices_.size() / 3; }

    bool validate() const noexcept;
    void clear() noexcept;

private:
    std::vector<Vertex> vertices_;
    std::vector<Index> indices_;
};

}
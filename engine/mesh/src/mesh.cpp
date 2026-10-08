#include "rogue/mesh/mesh.hpp"

namespace rogue::mesh {

bool Mesh::set_vertices(std::vector<Vertex> vertices) {
    vertices_ = std::move(vertices);
    return validate();
}

bool Mesh::set_indices(std::vector<Index> indices) {
    indices_ = std::move(indices);
    return validate();
}

bool Mesh::validate() const noexcept {
    if (indices_.size() % 3 != 0) return false;
    for (const Index index : indices_) {
        if (index >= vertices_.size()) return false;
    }
    return true;
}

void Mesh::clear() noexcept {
    vertices_.clear();
    indices_.clear();
}

}
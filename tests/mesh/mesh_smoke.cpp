#include <cassert>
#include <vector>
#include "rogue/mesh/mesh.hpp"

int main() {
    rogue::mesh::Mesh mesh;

    std::vector<rogue::mesh::Vertex> vertices(3);
    vertices[0].position[0] = 0.0f;
    vertices[1].position[0] = 1.0f;
    vertices[2].position[1] = 1.0f;

    assert(mesh.set_vertices(std::move(vertices)));
    assert(mesh.set_indices({0, 1, 2}));
    assert(mesh.vertex_count() == 3);
    assert(mesh.index_count() == 3);
    assert(mesh.triangle_count() == 1);
    assert(mesh.validate());

    assert(!mesh.set_indices({0, 1, 3}));
    mesh.clear();
    assert(mesh.vertex_count() == 0);
    return 0;
}

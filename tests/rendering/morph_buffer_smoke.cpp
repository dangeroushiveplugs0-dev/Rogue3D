#include "rogue/rendering/morph_buffer.hpp"

#include <cassert>

int main() {
    rogue::rendering::MorphBuffer buffer;
    assert(buffer.initialize({4, rogue::rendering::MorphBufferPrecision::Float16}));

    std::vector<float> dense(12, 0.25f);
    assert(buffer.set_dense_morph(7, dense));
    assert(buffer.morph_count() == 1);
    assert(buffer.find_morph(7)->element_count == 4);
    assert(buffer.packed_byte_size() == 24);

    std::vector<rogue::rendering::MorphDelta> sparse{
        {1, 0.5f, 0.0f, -0.5f},
        {3, 1.0f, 0.25f, 0.75f}
    };
    assert(buffer.set_sparse_morph(9, sparse));
    assert(buffer.find_morph(9)->storage == rogue::rendering::MorphStorageMode::Sparse);
    assert(buffer.find_morph(9)->element_count == 2);
    assert(buffer.find_morph(9)->bytes == 2 * (4 + 2 + 2 + 2));

    assert(buffer.set_active_morphs({{9, 0.8f}, {7, 0.2f}}));
    assert(buffer.active_morphs().size() == 2);
    assert(buffer.active_morphs()[0].morph_id == 7);
    assert(buffer.active_morphs()[1].morph_id == 9);

    return 0;
}

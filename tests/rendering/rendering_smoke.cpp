#include <cassert>
#include "rogue/rendering/backend.hpp"
#include "rogue/rendering/null_renderer.hpp"

int main() {
    rogue::rendering::NullRenderer renderer;
    assert(renderer.backend() == rogue::rendering::Backend::Null);
    assert(!renderer.is_initialized());
    assert(renderer.initialize());
    rogue::rendering::RenderFrame frame{};
    frame.delta_seconds = 1.0 / 60.0;
    frame.width = 1280;
    frame.height = 720;
    renderer.begin_frame(frame);
    renderer.end_frame();
    renderer.shutdown();
    assert(!renderer.is_initialized());
    return 0;
}

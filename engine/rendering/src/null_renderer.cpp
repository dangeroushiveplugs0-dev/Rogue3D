#include "rogue/rendering/null_renderer.hpp"

namespace rogue::rendering {

bool NullRenderer::initialize() { initialized_ = true; return true; }
void NullRenderer::begin_frame(const RenderFrame& frame) { if (initialized_) last_frame_ = frame; }
void NullRenderer::end_frame() { if (!initialized_) return; }
void NullRenderer::shutdown() { initialized_ = false; }

}
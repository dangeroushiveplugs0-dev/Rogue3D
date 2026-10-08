#include "rogue/core/engine.hpp"
namespace rogue::core {
bool Engine::initialize() { initialized_ = true; return true; }
void Engine::update(double delta_seconds) { if (!initialized_ || delta_seconds < 0.0) return; }
void Engine::shutdown() { initialized_ = false; }
}

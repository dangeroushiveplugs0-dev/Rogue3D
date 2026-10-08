#pragma once

#include "rogue/rendering/backend.hpp"

namespace rogue::rendering {

struct RenderFrame {
    double delta_seconds = 0.0;
    unsigned int width = 0;
    unsigned int height = 0;
};

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual bool initialize() = 0;
    virtual void begin_frame(const RenderFrame& frame) = 0;
    virtual void end_frame() = 0;
    virtual void shutdown() = 0;
    virtual Backend backend() const noexcept = 0;
    virtual bool is_initialized() const noexcept = 0;
};

}
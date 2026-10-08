#pragma once

#include "rogue/rendering/renderer.hpp"

namespace rogue::rendering {

class NullRenderer final : public Renderer {
public:
    bool initialize() override;
    void begin_frame(const RenderFrame& frame) override;
    void end_frame() override;
    void shutdown() override;
    Backend backend() const noexcept override { return Backend::Null; }
    bool is_initialized() const noexcept override { return initialized_; }
private:
    bool initialized_ = false;
    RenderFrame last_frame_{};
};

}
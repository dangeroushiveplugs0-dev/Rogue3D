#pragma once

namespace rogue::rendering {

enum class Backend { Null, Vulkan, WebGPU, OpenGL_ES, Metal, Direct3D12 };

constexpr const char* backend_name(Backend backend) noexcept {
    switch (backend) {
        case Backend::Vulkan: return "Vulkan";
        case Backend::WebGPU: return "WebGPU";
        case Backend::OpenGL_ES: return "OpenGL ES";
        case Backend::Metal: return "Metal";
        case Backend::Direct3D12: return "Direct3D 12";
        case Backend::Null: return "Null";
    }
    return "Unknown";
}

}
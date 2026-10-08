#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
struct ANativeWindow;
namespace rogue::runtime::android {
class VulkanAndroidSurface {
public:
 bool initialize(VkInstance,ANativeWindow*);
 void destroy() noexcept;
 VkSurfaceKHR surface()const noexcept{return surface_;}
private:
 VkInstance instance_=VK_NULL_HANDLE; VkSurfaceKHR surface_=VK_NULL_HANDLE;
};
}
#endif

#include "vulkan_android_surface.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <android/native_window.h>
namespace rogue::runtime::android {
bool VulkanAndroidSurface::initialize(VkInstance instance,ANativeWindow* window){destroy();if(!instance||!window)return false;instance_=instance;VkAndroidSurfaceCreateInfoKHR info{VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR};info.window=window;return vkCreateAndroidSurfaceKHR(instance_,&info,nullptr,&surface_)==VK_SUCCESS;}
void VulkanAndroidSurface::destroy()noexcept{if(instance_&&surface_)vkDestroySurfaceKHR(instance_,surface_,nullptr);surface_=VK_NULL_HANDLE;instance_=VK_NULL_HANDLE;}
}
#endif

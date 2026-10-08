#include "android_vulkan_renderer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <vector>
namespace rogue::runtime::android {
AndroidVulkanRenderer::~AndroidVulkanRenderer(){shutdown();}
bool AndroidVulkanRenderer::create_instance(){const char* extensions[]={"VK_KHR_surface","VK_KHR_android_surface"};VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};app.pApplicationName="RogueEngine";app.applicationVersion=1;app.pEngineName="RogueEngine";app.engineVersion=1;app.apiVersion=VK_API_VERSION_1_0;VkInstanceCreateInfo info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};info.pApplicationInfo=&app;info.enabledExtensionCount=2;info.ppEnabledExtensionNames=extensions;return vkCreateInstance(&info,nullptr,&instance_)==VK_SUCCESS;}
bool AndroidVulkanRenderer::initialize(ANativeWindow* window,std::uint32_t width,std::uint32_t height){shutdown();if(!window||!create_instance())return false;if(!surface_.initialize(instance_,window))return false;if(!device_.initialize(instance_,surface_.surface()))return false;rogue::rendering::vulkan::SwapchainConfig cfg{};cfg.width=width;cfg.height=height;cfg.vsync=true;if(!frames_.initialize(device_.physical_device(),device_.device(),surface_.surface(),device_.command_pool(),device_.graphics_queue(),device_.graphics_queue_family(),cfg))return false;return runtime_.initialize(instance_,device_.physical_device(),device_.device(),surface_.surface(),device_.graphics_queue(),device_.graphics_queue_family(),width,height);}
bool AndroidVulkanRenderer::resize(std::uint32_t width,std::uint32_t height){if(!device_.device())return false;return frames_.swapchain().recreate(width,height);}
bool AndroidVulkanRenderer::begin_frame(std::uint32_t& image){return frames_.begin_frame(image)==rogue::rendering::vulkan::FrameStatus::Rendered;}
bool AndroidVulkanRenderer::begin_render(){return frames_.begin_render();}
bool AndroidVulkanRenderer::end_render(){return frames_.end_render();}
bool AndroidVulkanRenderer::present(std::uint32_t image){return frames_.submit_and_present(image)==rogue::rendering::vulkan::FrameStatus::Rendered;}
void AndroidVulkanRenderer::shutdown()noexcept{if(device_.device())vkDeviceWaitIdle(device_.device());runtime_.destroy();frames_.destroy();device_.destroy();surface_.destroy();if(instance_)vkDestroyInstance(instance_,nullptr);instance_=VK_NULL_HANDLE;}
}
#endif

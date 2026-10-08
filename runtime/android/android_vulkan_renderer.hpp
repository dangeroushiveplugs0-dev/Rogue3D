#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include "rogue/rendering/vulkan/vulkan_device.hpp"
#include "rogue/rendering/vulkan/vulkan_frame_coordinator.hpp"
#include "rogue/rendering/vulkan/vulkan_mesh_pipeline.hpp"
#include "rogue/rendering/vulkan/vulkan_runtime.hpp"
#include "vulkan_android_surface.hpp"
struct ANativeWindow;
namespace rogue::runtime::android {
class AndroidVulkanRenderer {
public:
 ~AndroidVulkanRenderer();
 AndroidVulkanRenderer(const AndroidVulkanRenderer&)=delete;
 bool initialize(ANativeWindow*,std::uint32_t,std::uint32_t);
 bool resize(std::uint32_t,std::uint32_t);
 bool begin_frame(std::uint32_t&);
 bool begin_render();
 bool draw_mesh(rogue::rendering::vulkan::VulkanMeshPipeline&,const rogue::rendering::vulkan::VulkanMeshBuffer&,std::uint32_t,const rogue::rendering::vulkan::MeshFrameConstants&,const rogue::rendering::vulkan::MeshMaterialConstants&);
 bool end_render();
 bool present(std::uint32_t);
 void shutdown() noexcept;
 VkInstance instance()const noexcept{return instance_;}
 rogue::rendering::vulkan::VulkanDevice& device()noexcept{return device_;}
 rogue::rendering::vulkan::VulkanFrameCoordinator& frames()noexcept{return frames_;}
 rogue::rendering::vulkan::VulkanRuntime& runtime()noexcept{return runtime_;}
private:
 bool create_instance();
 VkInstance instance_=VK_NULL_HANDLE; std::uint32_t current_image_=0;
 VulkanAndroidSurface surface_;
 rogue::rendering::vulkan::VulkanDevice device_;
 rogue::rendering::vulkan::VulkanFrameCoordinator frames_;
 rogue::rendering::vulkan::VulkanRuntime runtime_;
};
}
#endif
#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include "rogue/rendering/vulkan/vulkan_frame.hpp"
#include "rogue/rendering/vulkan/vulkan_swapchain.hpp"
#include <cstdint>
namespace rogue::rendering::vulkan {
enum class FrameStatus { Rendered, RecreateRequired, Failed };
class VulkanFrameCoordinator {
public:
 bool initialize(VkPhysicalDevice,VkDevice,VkSurfaceKHR,VkCommandPool,VkQueue,std::uint32_t,const SwapchainConfig&);
 FrameStatus begin_frame(std::uint32_t&);
 bool begin_render();
 bool end_render();
 FrameStatus submit_and_present(std::uint32_t);
 void destroy() noexcept;
 VulkanFrameSync& sync() noexcept{return sync_;}
 VulkanSwapchain& swapchain() noexcept{return swapchain_;}
private:
 VkDevice device_=VK_NULL_HANDLE; VkQueue queue_=VK_NULL_HANDLE; VulkanFrameSync sync_; VulkanSwapchain swapchain_;
};
}
#endif
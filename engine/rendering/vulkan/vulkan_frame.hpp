#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#include <cstdint>
namespace rogue::rendering::vulkan {
struct VulkanFrameConfig { std::uint32_t frames_in_flight=2; };
class VulkanFrameSync {
public:
 ~VulkanFrameSync();
 VulkanFrameSync(const VulkanFrameSync&)=delete; VulkanFrameSync& operator=(const VulkanFrameSync&)=delete;
 bool initialize(VkDevice,VkCommandPool,std::uint32_t);
 bool begin(VkCommandBuffer);
 bool submit(VkQueue,VkCommandBuffer,VkSemaphore image_available,VkSemaphore render_finished);
 bool end();
 void destroy() noexcept;
 VkSemaphore image_available()const noexcept{return image_available_;}
 VkSemaphore render_finished()const noexcept{return render_finished_;}
 VkFence fence()const noexcept{return fence_;}
private:
 VkDevice device_=VK_NULL_HANDLE; VkCommandPool pool_=VK_NULL_HANDLE; VkCommandBuffer command_buffer_=VK_NULL_HANDLE; VkSemaphore image_available_=VK_NULL_HANDLE; VkSemaphore render_finished_=VK_NULL_HANDLE; VkFence fence_=VK_NULL_HANDLE; bool recording_=false;
};
}
#endif
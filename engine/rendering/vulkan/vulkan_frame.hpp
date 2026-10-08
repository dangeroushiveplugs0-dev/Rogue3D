#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#include <cstdint>
namespace rogue::rendering::vulkan {
class VulkanFrameSync {
public:
 ~VulkanFrameSync();
 VulkanFrameSync(const VulkanFrameSync&)=delete; VulkanFrameSync& operator=(const VulkanFrameSync&)=delete;
 bool initialize(VkDevice,VkCommandPool);
 bool wait() const;
 bool begin();
 bool end();
 bool submit(VkQueue,VkSemaphore,VkSemaphore);
 void destroy() noexcept;
 VkCommandBuffer command_buffer()const noexcept{return command_buffer_;}
 VkSemaphore image_available()const noexcept{return image_available_;}
 VkSemaphore render_finished()const noexcept{return render_finished_;}
 VkFence fence()const noexcept{return fence_;}
private:
 VkDevice device_=VK_NULL_HANDLE; VkCommandPool pool_=VK_NULL_HANDLE; VkCommandBuffer command_buffer_=VK_NULL_HANDLE;
 VkSemaphore image_available_=VK_NULL_HANDLE,render_finished_=VK_NULL_HANDLE; VkFence fence_=VK_NULL_HANDLE; bool recording_=false;
};
}
#endif
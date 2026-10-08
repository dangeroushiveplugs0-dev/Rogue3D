#pragma once
#include <cstdint>
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#endif
namespace rogue::rendering::vulkan {
#if defined(ROGUE_ENABLE_VULKAN)
class VulkanUploadBuffer {
public:
 ~VulkanUploadBuffer();
 VulkanUploadBuffer(const VulkanUploadBuffer&)=delete;
 VulkanUploadBuffer& operator=(const VulkanUploadBuffer&)=delete;
 bool initialize(VkPhysicalDevice,VkDevice,VkDeviceSize,VkBufferUsageFlags);
 bool upload(const void*,VkDeviceSize);
 bool copy_to(VkCommandBuffer,VkBuffer,VkDeviceSize=0)const;
 VkBuffer buffer()const noexcept{return buffer_;}
 VkDeviceSize size()const noexcept{return size_;}
 void destroy()noexcept;
private:
 std::uint32_t memory_type(std::uint32_t,VkMemoryPropertyFlags)const noexcept;
 VkPhysicalDevice physical_device_=VK_NULL_HANDLE;VkDevice device_=VK_NULL_HANDLE;VkBuffer buffer_=VK_NULL_HANDLE;VkDeviceMemory memory_=VK_NULL_HANDLE;VkDeviceSize size_=0;
};
#endif
}
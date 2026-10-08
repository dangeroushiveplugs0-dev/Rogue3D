#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#endif
namespace rogue::rendering::vulkan {
#if defined(ROGUE_ENABLE_VULKAN)
bool record_buffer_copy_and_barrier(VkCommandBuffer,VkBuffer,VkBuffer,VkDeviceSize);
#endif
}
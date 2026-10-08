#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#include <cstdint>
namespace rogue::rendering::vulkan {
class VulkanDevice {
public:
 ~VulkanDevice();
 bool initialize(VkInstance,VkSurfaceKHR);
 void destroy() noexcept;
 VkPhysicalDevice physical_device()const noexcept{return physical_device_;}
 VkDevice device()const noexcept{return device_;}
 VkQueue graphics_queue()const noexcept{return graphics_queue_;}
 std::uint32_t graphics_queue_family()const noexcept{return graphics_family_;}
 VkCommandPool command_pool()const noexcept{return command_pool_;}
private:
 bool pick_physical(VkSurfaceKHR);
 bool create_logical();
 bool create_command_pool();
 VkInstance instance_=VK_NULL_HANDLE; VkPhysicalDevice physical_device_=VK_NULL_HANDLE; VkDevice device_=VK_NULL_HANDLE; VkQueue graphics_queue_=VK_NULL_HANDLE; VkCommandPool command_pool_=VK_NULL_HANDLE; std::uint32_t graphics_family_=0;
};
}
#endif
#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>
namespace rogue::rendering::vulkan {
class VulkanRuntime {
public:
 ~VulkanRuntime();
 VulkanRuntime(const VulkanRuntime&)=delete; VulkanRuntime& operator=(const VulkanRuntime&)=delete;
 bool initialize(VkPhysicalDevice,VkDevice,VkSurfaceKHR,VkQueue,std::uint32_t,VkFormat,const std::vector<VkImageView>&,std::uint32_t,std::uint32_t);
 VkRenderPass render_pass()const noexcept{return render_pass_;}
 VkExtent2D extent()const noexcept{return extent_;}
 VkFormat color_format()const noexcept{return color_format_;}
 VkFormat depth_format()const noexcept{return depth_format_;}
 bool begin(VkCommandBuffer,std::uint32_t,VkClearColorValue);
 bool end(VkCommandBuffer);
 void destroy() noexcept;
private:
 bool create_render_pass(); bool create_depth_target(); bool create_framebuffers();
 bool find_depth_format();
 std::uint32_t memory_type(std::uint32_t,VkMemoryPropertyFlags)const noexcept;
 VkPhysicalDevice physical_device_=VK_NULL_HANDLE; VkDevice device_=VK_NULL_HANDLE; VkSurfaceKHR surface_=VK_NULL_HANDLE; VkQueue queue_=VK_NULL_HANDLE; std::uint32_t queue_family_=0;
 VkRenderPass render_pass_=VK_NULL_HANDLE; std::vector<VkFramebuffer> framebuffers_; VkImage depth_image_=VK_NULL_HANDLE; VkDeviceMemory depth_memory_=VK_NULL_HANDLE; VkImageView depth_view_=VK_NULL_HANDLE;
 VkFormat color_format_=VK_FORMAT_B8G8R8A8_UNORM; VkFormat depth_format_=VK_FORMAT_D32_SFLOAT; VkExtent2D extent_{1,1}; std::vector<VkImageView> color_views_;
};
}
#endif
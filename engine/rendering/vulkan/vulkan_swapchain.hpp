#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>
namespace rogue::rendering::vulkan {
struct SwapchainConfig { std::uint32_t width=1,height=1; bool vsync=true; };
class VulkanSwapchain {
public:
 ~VulkanSwapchain();
 VulkanSwapchain(const VulkanSwapchain&)=delete; VulkanSwapchain& operator=(const VulkanSwapchain&)=delete;
 bool initialize(VkPhysicalDevice,VkDevice,VkSurfaceKHR,VkQueue,std::uint32_t,const SwapchainConfig&);
 bool acquire(VkSemaphore,VkFence,std::uint32_t&);
 VkResult present(std::uint32_t,VkSemaphore);
 bool recreate(std::uint32_t,std::uint32_t);
 void destroy() noexcept;
 VkSwapchainKHR handle()const noexcept{return swapchain_;}
 VkFormat format()const noexcept{return format_;}
 VkExtent2D extent()const noexcept{return extent_;}
 const std::vector<VkImageView>& image_views()const noexcept{return views_;}
private:
 bool create(); bool create_views(); VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR>&)const; VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>&)const; VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR&)const;
 VkPhysicalDevice physical_device_=VK_NULL_HANDLE; VkDevice device_=VK_NULL_HANDLE; VkSurfaceKHR surface_=VK_NULL_HANDLE; VkQueue queue_=VK_NULL_HANDLE; std::uint32_t queue_family_=0; VkSwapchainKHR swapchain_=VK_NULL_HANDLE; VkFormat format_=VK_FORMAT_B8G8R8A8_UNORM; VkColorSpaceKHR color_space_=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; VkExtent2D extent_{1,1}; std::vector<VkImage> images_; std::vector<VkImageView> views_; SwapchainConfig config_{};
};
}
#endif
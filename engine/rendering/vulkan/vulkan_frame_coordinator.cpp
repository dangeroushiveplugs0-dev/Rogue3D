#include "rogue/rendering/vulkan/vulkan_frame_coordinator.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
namespace rogue::rendering::vulkan {
bool VulkanFrameCoordinator::initialize(VkPhysicalDevice p,VkDevice d,VkSurfaceKHR s,VkCommandPool pool,VkQueue q,std::uint32_t qf,const SwapchainConfig& c){destroy();device_=d;queue_=q;return device_&&sync_.initialize(d,pool)&&swapchain_.initialize(p,d,s,q,qf,c);}
FrameStatus VulkanFrameCoordinator::begin_frame(std::uint32_t& index){if(!sync_.wait())return FrameStatus::Failed;auto r=swapchain_.acquire(sync_.image_available(),sync_.fence(),index);if(r==VK_ERROR_OUT_OF_DATE_KHR)return FrameStatus::RecreateRequired;if(r!=VK_SUCCESS&&r!=VK_SUBOPTIMAL_KHR)return FrameStatus::Failed;return FrameStatus::Rendered;}
bool VulkanFrameCoordinator::begin_render(){return sync_.begin();}
bool VulkanFrameCoordinator::end_render(){return sync_.end();}
FrameStatus VulkanFrameCoordinator::submit_and_present(std::uint32_t index){if(!sync_.submit(queue_,sync_.image_available(),sync_.render_finished()))return FrameStatus::Failed;auto r=swapchain_.present(index,sync_.render_finished());if(r==VK_ERROR_OUT_OF_DATE_KHR||r==VK_SUBOPTIMAL_KHR)return FrameStatus::RecreateRequired;return r==VK_SUCCESS?FrameStatus::Rendered:FrameStatus::Failed;}
void VulkanFrameCoordinator::destroy()noexcept{sync_.destroy();swapchain_.destroy();device_=VK_NULL_HANDLE;queue_=VK_NULL_HANDLE;}
}
#endif
#include "rogue/rendering/vulkan/vulkan_frame_coordinator.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
namespace rogue::rendering::vulkan {
bool VulkanFrameCoordinator::initialize(VkPhysicalDevice pd,VkDevice d,VkSurfaceKHR surface,VkCommandPool pool,VkQueue q,std::uint32_t qf,const SwapchainConfig& c){destroy();device_=d;queue_=q;return pd&&device_&&surface&&queue_&&sync_.initialize(d,pool)&&swapchain_.initialize(pd,d,surface,q,qf,c);}
FrameStatus VulkanFrameCoordinator::begin_frame(std::uint32_t& image_index){if(!sync_.wait())return FrameStatus::Failed;VkResult r=swapchain_.acquire(sync_.image_available(),VK_NULL_HANDLE,image_index)?VK_SUCCESS:VK_ERROR_OUT_OF_DATE_KHR;if(r==VK_ERROR_OUT_OF_DATE_KHR)return FrameStatus::RecreateRequired;return FrameStatus::Rendered;}
bool VulkanFrameCoordinator::begin_render(){return sync_.begin();}
bool VulkanFrameCoordinator::end_render(){return sync_.end();}
FrameStatus VulkanFrameCoordinator::submit_and_present(std::uint32_t image_index){if(!sync_.submit(queue_,sync_.image_available(),sync_.render_finished()))return FrameStatus::Failed;VkResult r=swapchain_.present(image_index,sync_.render_finished());if(r==VK_ERROR_OUT_OF_DATE_KHR||r==VK_SUBOPTIMAL_KHR)return FrameStatus::RecreateRequired;return r==VK_SUCCESS?FrameStatus::Rendered:FrameStatus::Failed;}
void VulkanFrameCoordinator::destroy()noexcept{swapchain_.destroy();sync_.destroy();device_=VK_NULL_HANDLE;queue_=VK_NULL_HANDLE;}
}
#endif
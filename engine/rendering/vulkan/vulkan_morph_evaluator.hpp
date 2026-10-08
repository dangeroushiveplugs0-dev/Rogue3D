#pragma once
#include "rogue/rendering/vulkan/vulkan_morph_buffer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#endif
namespace rogue::rendering::vulkan {
#if defined(ROGUE_ENABLE_VULKAN)
class VulkanMorphEvaluator {
public:
 ~VulkanMorphEvaluator();
 bool initialize(VkPhysicalDevice,VkDevice,VkShaderModule,const VulkanMorphBuffer&,const MorphBuffer&);
 bool update_morphs(const MorphBuffer&);
 bool dispatch(VkCommandBuffer,VkBuffer,VkBuffer,std::uint32_t);
 void destroy() noexcept;
 bool is_initialized() const noexcept { return pipeline_!=VK_NULL_HANDLE; }
private:
 bool make_buffer(VkDeviceSize,VkBuffer*,VkDeviceMemory*);
 bool upload(VkBuffer,VkDeviceMemory,VkDeviceSize,const void*,VkDeviceSize);
 std::uint32_t memory_type(std::uint32_t,VkMemoryPropertyFlags) const noexcept;
 VkPhysicalDevice physical_device_=VK_NULL_HANDLE; VkDevice device_=VK_NULL_HANDLE; VkShaderModule shader_=VK_NULL_HANDLE;
 VkDescriptorSetLayout layout_=VK_NULL_HANDLE; VkPipelineLayout pipeline_layout_=VK_NULL_HANDLE; VkPipeline pipeline_=VK_NULL_HANDLE;
 VkDescriptorPool pool_=VK_NULL_HANDLE; VkDescriptorSet set_=VK_NULL_HANDLE; VkBuffer morph_payload_=VK_NULL_HANDLE;
 VkBuffer slices_=VK_NULL_HANDLE; VkDeviceMemory slices_memory_=VK_NULL_HANDLE; VkDeviceSize slices_size_=0;
 VkBuffer active_=VK_NULL_HANDLE; VkDeviceMemory active_memory_=VK_NULL_HANDLE; VkDeviceSize active_size_=0;
 std::uint32_t slice_count_=0; std::uint32_t active_count_=0;
};
#endif
}
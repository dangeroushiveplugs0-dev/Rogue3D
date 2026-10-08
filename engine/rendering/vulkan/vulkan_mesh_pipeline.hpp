#pragma once
#include "rogue/rendering/vulkan/vulkan_mesh_buffer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#endif
namespace rogue::rendering::vulkan {
#if defined(ROGUE_ENABLE_VULKAN)
struct alignas(16) MeshFrameConstants { float view_projection[16]{}; float model[16]{}; float camera_position[4]{}; };
struct alignas(16) MeshMaterialConstants { float base_color[4]{1,1,1,1}; float emissive[4]{}; float metallic=0; float roughness=0.5f; float reflectiveness=0; float wetness=0; float organic=0; float dryness=0; float normal_strength=1; float occlusion=1; };
class VulkanMeshPipeline { public: ~VulkanMeshPipeline(); VulkanMeshPipeline(const VulkanMeshPipeline&)=delete; VulkanMeshPipeline& operator=(const VulkanMeshPipeline&)=delete; bool initialize(VkPhysicalDevice,VkDevice,VkRenderPass,VkShaderModule,VkShaderModule); bool bind(VkCommandBuffer,const VulkanMeshBuffer&,std::uint32_t,const MeshFrameConstants&,const MeshMaterialConstants&); void destroy() noexcept; bool is_initialized()const noexcept{return pipeline_!=VK_NULL_HANDLE;} private: std::uint32_t memory_type(std::uint32_t,VkMemoryPropertyFlags)const noexcept; VkPhysicalDevice physical_device_=VK_NULL_HANDLE; VkDevice device_=VK_NULL_HANDLE; VkRenderPass render_pass_=VK_NULL_HANDLE; VkDescriptorSetLayout layout_=VK_NULL_HANDLE; VkPipelineLayout pipeline_layout_=VK_NULL_HANDLE; VkPipeline pipeline_=VK_NULL_HANDLE; VkDescriptorPool pool_=VK_NULL_HANDLE; VkDescriptorSet set_=VK_NULL_HANDLE; VkBuffer frame_buffer_=VK_NULL_HANDLE; VkDeviceMemory frame_memory_=VK_NULL_HANDLE; VkBuffer material_buffer_=VK_NULL_HANDLE; VkDeviceMemory material_memory_=VK_NULL_HANDLE; };
#endif }
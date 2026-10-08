#pragma once
#include "rogue/mesh/mesh.hpp"
#include "rogue/rendering/vulkan/vulkan_device_buffer.hpp"
#include "rogue/rendering/vulkan/vulkan_upload_buffer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#endif
namespace rogue::rendering::vulkan {
#if defined(ROGUE_ENABLE_VULKAN)
class VulkanMeshBuffer {
public:
    ~VulkanMeshBuffer();
    VulkanMeshBuffer(const VulkanMeshBuffer&)=delete;
    VulkanMeshBuffer& operator=(const VulkanMeshBuffer&)=delete;
    bool initialize(VkPhysicalDevice,VkDevice,const rogue::mesh::Mesh&);
    bool update(VkCommandBuffer,const rogue::mesh::Mesh&);
    void destroy() noexcept;
    bool is_initialized()const noexcept{return vertex_.buffer()!=VK_NULL_HANDLE&&index_.buffer()!=VK_NULL_HANDLE;}
    VkBuffer vertex_buffer()const noexcept{return vertex_.buffer();}
    VkBuffer index_buffer()const noexcept{return index_.buffer();}
    VkDeviceSize vertex_bytes()const noexcept{return vertex_.size();}
    VkDeviceSize index_bytes()const noexcept{return index_.size();}
    std::uint32_t index_count()const noexcept{return index_count_;}
private:
    VkPhysicalDevice physical_device_=VK_NULL_HANDLE;
    VkDevice device_=VK_NULL_HANDLE;
    VulkanDeviceBuffer vertex_;
    VulkanDeviceBuffer index_;
    VulkanUploadBuffer staging_;
    std::uint32_t index_count_=0;
};
#endif
}
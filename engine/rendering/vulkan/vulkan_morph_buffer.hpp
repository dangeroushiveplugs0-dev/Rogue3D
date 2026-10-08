#pragma once
#include "rogue/rendering/morph_buffer.hpp"
#include <cstdint>
#include <vector>
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#endif
namespace rogue::rendering::vulkan {
struct GpuMorphSlice { std::uint32_t morph_id=0, offset_bytes=0, element_count=0, storage_mode=0, precision=0; };
struct GpuActiveMorph { std::uint32_t morph_id=0; float weight=0.0f; };
#if defined(ROGUE_ENABLE_VULKAN)
class VulkanMorphBuffer {
public:
    ~VulkanMorphBuffer();
    VulkanMorphBuffer(const VulkanMorphBuffer&)=delete;
    VulkanMorphBuffer& operator=(const VulkanMorphBuffer&)=delete;
    bool initialize(VkPhysicalDevice physical_device, VkDevice device, VkQueue transfer_queue,
                    std::uint32_t transfer_queue_family, const MorphBuffer& source);
    bool update(VkCommandBuffer command_buffer, const MorphBuffer& source);
    void destroy() noexcept;
    bool is_initialized() const noexcept { return device_!=VK_NULL_HANDLE && buffer_!=VK_NULL_HANDLE; }
    VkBuffer buffer() const noexcept { return buffer_; }
    VkDeviceSize size() const noexcept { return size_; }
    const std::vector<GpuMorphSlice>& slices() const noexcept { return slices_; }
    const std::vector<GpuActiveMorph>& active_morphs() const noexcept { return active_; }
private:
    bool create_buffer(VkDeviceSize size);
    bool upload(const std::vector<std::uint8_t>& bytes);
    std::uint32_t find_memory_type(std::uint32_t type_bits, VkMemoryPropertyFlags properties) const noexcept;
    VkPhysicalDevice physical_device_=VK_NULL_HANDLE; VkDevice device_=VK_NULL_HANDLE;
    VkQueue transfer_queue_=VK_NULL_HANDLE; std::uint32_t transfer_queue_family_=0;
    VkBuffer buffer_=VK_NULL_HANDLE; VkDeviceMemory memory_=VK_NULL_HANDLE; VkDeviceSize size_=0;
    std::vector<GpuMorphSlice> slices_; std::vector<GpuActiveMorph> active_;
};
#endif
}
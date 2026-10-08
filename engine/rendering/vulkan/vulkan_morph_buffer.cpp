#include "rogue/rendering/vulkan/vulkan_morph_buffer.hpp"

#if defined(ROGUE_ENABLE_VULKAN)

#include <cstring>
#include <limits>

namespace rogue::rendering::vulkan {

VulkanMorphBuffer::~VulkanMorphBuffer() {
    destroy();
}

std::uint32_t VulkanMorphBuffer::find_memory_type(
    std::uint32_t type_bits,
    VkMemoryPropertyFlags properties) const noexcept {
    VkPhysicalDeviceMemoryProperties memory_properties{};
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties);

    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((type_bits & (1u << i)) != 0 &&
            (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return std::numeric_limits<std::uint32_t>::max();
}

bool VulkanMorphBuffer::create_buffer(VkDeviceSize size) {
    if (size == 0) {
        return false;
    }

    VkBufferCreateInfo info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    info.size = size;
    info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_, &info, nullptr, &buffer_) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements requirements{};
    vkGetBufferMemoryRequirements(device_, buffer_, &requirements);

    const std::uint32_t memory_type = find_memory_type(
        requirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (memory_type == std::numeric_limits<std::uint32_t>::max()) {
        vkDestroyBuffer(device_, buffer_, nullptr);
        buffer_ = VK_NULL_HANDLE;
        return false;
    }

    VkMemoryAllocateInfo allocation{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocation.allocationSize = requirements.size;
    allocation.memoryTypeIndex = memory_type;

    if (vkAllocateMemory(device_, &allocation, nullptr, &memory_) != VK_SUCCESS) {
        vkDestroyBuffer(device_, buffer_, nullptr);
        buffer_ = VK_NULL_HANDLE;
        return false;
    }

    if (vkBindBufferMemory(device_, buffer_, memory_, 0) != VK_SUCCESS) {
        vkFreeMemory(device_, memory_, nullptr);
        vkDestroyBuffer(device_, buffer_, nullptr);
        memory_ = VK_NULL_HANDLE;
        buffer_ = VK_NULL_HANDLE;
        return false;
    }

    size_ = size;
    return true;
}

bool VulkanMorphBuffer::upload(const std::vector<std::uint8_t>& bytes) {
    if (bytes.empty() || bytes.size() > size_) {
        return false;
    }

    void* mapped = nullptr;
    if (vkMapMemory(device_, memory_, 0, bytes.size(), 0, &mapped) != VK_SUCCESS) {
        return false;
    }

    std::memcpy(mapped, bytes.data(), bytes.size());
    vkUnmapMemory(device_, memory_);
    return true;
}

bool VulkanMorphBuffer::initialize(VkPhysicalDevice pd,VkDevice d,VkQueue q,std::uint32_t qf,const MorphBuffer& source) {
    destroy();
    if (!pd || !d || !q || source.packed_data().empty()) return false;
    physical_device_=pd; device_=d; transfer_queue_=q; transfer_queue_family_=qf;
    PackedGpuMorphData packed;
    if (!pack_morph_buffer_for_gpu(source,packed) || packed.bytes.empty() || !create_buffer(packed.bytes.size())) { destroy(); return false; }
    slices_.clear(); slices_.reserve(source.slices().size());
    for (std::size_t i=0;i<source.slices().size();++i) { const auto&s=source.slices()[i]; slices_.push_back({s.morph_id,packed.offsets[i],s.element_count,(std::uint32_t)s.storage,(std::uint32_t)s.precision}); }
    active_.clear(); active_.reserve(source.active_morphs().size());
    for (const auto&a:source.active_morphs()) active_.push_back({a.morph_id,a.weight});
    return true;
}

bool VulkanMorphBuffer::update(VkCommandBuffer command_buffer,const MorphBuffer& source) {
    if (!is_initialized() || !command_buffer) return false;
    PackedGpuMorphData packed;
    if (!pack_morph_buffer_for_gpu(source,packed) || packed.bytes.empty() || packed.bytes.size()>size_) return false;
    VulkanUploadBuffer upload;
    if (!upload.initialize(physical_device_,device_,packed.bytes.size(),VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) ||
        !upload.upload(packed.bytes.data(),packed.bytes.size()) ||
        !record_buffer_copy_and_barrier(command_buffer,upload.buffer(),buffer_,packed.bytes.size())) return false;
    slices_.clear(); slices_.reserve(source.slices().size());
    for (std::size_t i=0;i<source.slices().size();++i) { const auto&s=source.slices()[i]; slices_.push_back({s.morph_id,packed.offsets[i],s.element_count,(std::uint32_t)s.storage,(std::uint32_t)s.precision}); }
    active_.clear(); active_.reserve(source.active_morphs().size());
    for (const auto&a:source.active_morphs()) active_.push_back({a.morph_id,a.weight});
    return true;
}

void VulkanMorphBuffer::destroy() noexcept {
    if (device_ != VK_NULL_HANDLE) {
        if (buffer_ != VK_NULL_HANDLE) {
            vkDestroyBuffer(device_, buffer_, nullptr);
        }
        if (memory_ != VK_NULL_HANDLE) {
            vkFreeMemory(device_, memory_, nullptr);
        }
    }

    buffer_ = VK_NULL_HANDLE;
    memory_ = VK_NULL_HANDLE;
    size_ = 0;
    slices_.clear();
    active_.clear();
    physical_device_ = VK_NULL_HANDLE;
    device_ = VK_NULL_HANDLE;
    transfer_queue_ = VK_NULL_HANDLE;
    transfer_queue_family_ = 0;
}

} // namespace rogue::rendering::vulkan

#endif

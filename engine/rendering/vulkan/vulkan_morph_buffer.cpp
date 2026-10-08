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

bool VulkanMorphBuffer::initialize(
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkQueue transfer_queue,
    std::uint32_t transfer_queue_family,
    const MorphBuffer& source) {
    destroy();

    if (physical_device == VK_NULL_HANDLE ||
        device == VK_NULL_HANDLE ||
        transfer_queue == VK_NULL_HANDLE ||
        source.packed_data().empty()) {
        return false;
    }

    physical_device_ = physical_device;
    device_ = device;
    transfer_queue_ = transfer_queue;
    transfer_queue_family_ = transfer_queue_family;

    if (!create_buffer(static_cast<VkDeviceSize>(source.packed_data().size()))) {
        destroy();
        return false;
    }

    if (!upload(source.packed_data())) {
        destroy();
        return false;
    }

    slices_.clear();
    slices_.reserve(source.slices().size());

    for (const auto& source_slice : source.slices()) {
        slices_.push_back({
            source_slice.morph_id,
            static_cast<std::uint32_t>(source_slice.offset),
            source_slice.element_count,
            static_cast<std::uint32_t>(source_slice.storage),
            static_cast<std::uint32_t>(source_slice.precision)
        });
    }

    active_.clear();
    active_.reserve(source.active_morphs().size());

    for (const auto& active : source.active_morphs()) {
        active_.push_back({active.morph_id, active.weight});
    }

    return true;
}

bool VulkanMorphBuffer::update(const MorphBuffer& source) {
    if (!is_initialized() ||
        source.packed_data().size() > size_ ||
        !upload(source.packed_data())) {
        return false;
    }

    slices_.clear();
    slices_.reserve(source.slices().size());

    for (const auto& source_slice : source.slices()) {
        slices_.push_back({
            source_slice.morph_id,
            static_cast<std::uint32_t>(source_slice.offset),
            source_slice.element_count,
            static_cast<std::uint32_t>(source_slice.storage),
            static_cast<std::uint32_t>(source_slice.precision)
        });
    }

    active_.clear();
    active_.reserve(source.active_morphs().size());

    for (const auto& active : source.active_morphs()) {
        active_.push_back({active.morph_id, active.weight});
    }

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

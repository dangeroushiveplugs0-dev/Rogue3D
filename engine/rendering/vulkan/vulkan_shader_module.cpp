#include "vulkan_shader_module.hpp"

#if defined(ROGUE_ENABLE_VULKAN)
#include <fstream>
#include <cstring>
#include <limits>

namespace rogue::rendering::vulkan {

namespace {
constexpr std::uint32_t kSpirvMagic = 0x07230203u;

bool read_file(std::string_view path, std::vector<std::uint32_t>& words) {
    if (path.empty()) {
        return false;
    }

    std::ifstream file(std::string(path), std::ios::binary | std::ios::ate);
    if (!file) {
        return false;
    }

    const std::streamoff size = file.tellg();
    if (size <= 0 || (size % static_cast<std::streamoff>(sizeof(std::uint32_t))) != 0) {
        return false;
    }

    const auto word_count = static_cast<std::uint64_t>(size / sizeof(std::uint32_t));
    if (word_count > std::numeric_limits<std::size_t>::max()) {
        return false;
    }

    words.resize(static_cast<std::size_t>(word_count));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(words.data()), size);
    return file.good() || file.gcount() == size;
}
} // namespace

VulkanShaderModule::~VulkanShaderModule() {
    destroy();
}

bool VulkanShaderModule::validate_spirv(const std::vector<std::uint32_t>& spirv) noexcept {
    return spirv.size() >= 5u && spirv[0] == kSpirvMagic;
}

bool VulkanShaderModule::initialize_bytes(VkDevice device, const std::uint8_t* data, std::size_t size_bytes) {
    if (data == nullptr || size_bytes == 0 || (size_bytes % sizeof(std::uint32_t)) != 0) return false;
    std::vector<std::uint32_t> spirv(size_bytes / sizeof(std::uint32_t));
    std::memcpy(spirv.data(), data, size_bytes);
    return initialize(device, spirv);
}

bool VulkanShaderModule::initialize(VkDevice device, const std::vector<std::uint32_t>& spirv) {
    destroy();

    if (device == VK_NULL_HANDLE || !validate_spirv(spirv)) {
        return false;
    }

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = spirv.size() * sizeof(std::uint32_t);
    create_info.pCode = spirv.data();

    VkShaderModule module = VK_NULL_HANDLE;
    if (vkCreateShaderModule(device, &create_info, nullptr, &module) != VK_SUCCESS) {
        return false;
    }

    device_ = device;
    module_ = module;
    return true;
}

bool VulkanShaderModule::load_file(VkDevice device, std::string_view path) {
    std::vector<std::uint32_t> spirv;
    if (!read_file(path, spirv)) {
        return false;
    }
    return initialize(device, spirv);
}

void VulkanShaderModule::destroy() noexcept {
    if (module_ != VK_NULL_HANDLE && device_ != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device_, module_, nullptr);
    }
    module_ = VK_NULL_HANDLE;
    device_ = VK_NULL_HANDLE;
}

} // namespace rogue::rendering::vulkan
#endif

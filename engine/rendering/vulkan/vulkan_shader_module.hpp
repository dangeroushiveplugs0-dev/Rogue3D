#pragma once
#if defined(ROGUE_ENABLE_VULKAN)
#include <vulkan/vulkan.h>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace rogue::rendering::vulkan {

class VulkanShaderModule {
public:
    VulkanShaderModule() = default;
    ~VulkanShaderModule();

    VulkanShaderModule(const VulkanShaderModule&) = delete;
    VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;

    bool initialize(VkDevice device, const std::vector<std::uint32_t>& spirv);
    bool load_file(VkDevice device, std::string_view path);

    static bool validate_spirv(const std::vector<std::uint32_t>& spirv) noexcept;

    VkShaderModule handle() const noexcept { return module_; }
    bool is_initialized() const noexcept { return module_ != VK_NULL_HANDLE; }

    void destroy() noexcept;

private:
    VkDevice device_ = VK_NULL_HANDLE;
    VkShaderModule module_ = VK_NULL_HANDLE;
};

} // namespace rogue::rendering::vulkan
#endif

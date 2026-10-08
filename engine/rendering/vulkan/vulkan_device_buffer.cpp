#include "rogue/rendering/vulkan/vulkan_device_buffer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <limits>
namespace rogue::rendering::vulkan {
VulkanDeviceBuffer::~VulkanDeviceBuffer(){destroy();}
std::uint32_t VulkanDeviceBuffer::memory_type(std::uint32_t bits,VkMemoryPropertyFlags props)const noexcept{VkPhysicalDeviceMemoryProperties p{};vkGetPhysicalDeviceMemoryProperties(physical_device_,&p);for(std::uint32_t i=0;i<p.memoryTypeCount;++i)if((bits&(1u<<i))&&(p.memoryTypes[i].propertyFlags&props)==props)return i;return std::numeric_limits<std::uint32_t>::max();}
bool VulkanDeviceBuffer::initialize(VkPhysicalDevice pd,VkDevice d,VkDeviceSize n,VkBufferUsageFlags usage){destroy();if(!pd||!d||!n)return false;physical_device_=pd;device_=d;size_=n;VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};bi.size=n;bi.usage=usage|VK_BUFFER_USAGE_TRANSFER_DST_BIT;bi.sharingMode=VK_SHARING_MODE_EXCLUSIVE;if(vkCreateBuffer(d,&bi,nullptr,&buffer_)!=VK_SUCCESS){destroy();return false;}VkMemoryRequirements r{};vkGetBufferMemoryRequirements(d,buffer_,&r);auto mt=memory_type(r.memoryTypeBits,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);if(mt==std::numeric_limits<std::uint32_t>::max()){destroy();return false;}VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};ai.allocationSize=r.size;ai.memoryTypeIndex=mt;if(vkAllocateMemory(d,&ai,nullptr,&memory_)!=VK_SUCCESS||vkBindBufferMemory(d,buffer_,memory_,0)!=VK_SUCCESS){destroy();return false;}return true;}
void VulkanDeviceBuffer::destroy()noexcept{if(device_&&buffer_)vkDestroyBuffer(device_,buffer_,nullptr);if(device_&&memory_)vkFreeMemory(device_,memory_,nullptr);buffer_=VK_NULL_HANDLE;memory_=VK_NULL_HANDLE;size_=0;device_=VK_NULL_HANDLE;physical_device_=VK_NULL_HANDLE;}
}
#endif
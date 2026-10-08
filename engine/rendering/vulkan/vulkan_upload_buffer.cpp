#include "rogue/rendering/vulkan/vulkan_upload_buffer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <cstring>
#include <limits>
namespace rogue::rendering::vulkan {
VulkanUploadBuffer::~VulkanUploadBuffer(){destroy();}
std::uint32_t VulkanUploadBuffer::memory_type(std::uint32_t bits,VkMemoryPropertyFlags props)const noexcept{VkPhysicalDeviceMemoryProperties p{};vkGetPhysicalDeviceMemoryProperties(physical_device_,&p);for(std::uint32_t i=0;i<p.memoryTypeCount;++i)if((bits&(1u<<i))&&(p.memoryTypes[i].propertyFlags&props)==props)return i;return std::numeric_limits<std::uint32_t>::max();}
bool VulkanUploadBuffer::initialize(VkPhysicalDevice pd,VkDevice d,VkDeviceSize n,VkBufferUsageFlags usage){destroy();if(!pd||!d||!n)return false;physical_device_=pd;device_=d;size_=n;VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};bi.size=n;bi.usage=usage|VK_BUFFER_USAGE_TRANSFER_SRC_BIT;bi.sharingMode=VK_SHARING_MODE_EXCLUSIVE;if(vkCreateBuffer(d,&bi,nullptr,&buffer_)!=VK_SUCCESS){destroy();return false;}VkMemoryRequirements r{};vkGetBufferMemoryRequirements(d,buffer_,&r);auto mt=memory_type(r.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);if(mt==std::numeric_limits<std::uint32_t>::max()){destroy();return false;}VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};ai.allocationSize=r.size;ai.memoryTypeIndex=mt;if(vkAllocateMemory(d,&ai,nullptr,&memory_)!=VK_SUCCESS||vkBindBufferMemory(d,buffer_,memory_,0)!=VK_SUCCESS){destroy();return false;}return true;}
bool VulkanUploadBuffer::upload(const void*data,VkDeviceSize n){if(!memory_||!data||n>size_)return false;void*p=nullptr;if(vkMapMemory(device_,memory_,0,n,0,&p)!=VK_SUCCESS)return false;std::memcpy(p,data,(std::size_t)n);vkUnmapMemory(device_,memory_);return true;}
bool VulkanUploadBuffer::copy_to(VkCommandBuffer cmd,VkBuffer dst,VkDeviceSize n)const{if(!cmd||!buffer_||!dst)return false;if(n==0)n=size_;if(n>size_)return false;VkBufferCopy r{0,0,n};vkCmdCopyBuffer(cmd,buffer_,dst,1,&r);return true;}
void VulkanUploadBuffer::destroy()noexcept{if(device_&&buffer_)vkDestroyBuffer(device_,buffer_,nullptr);if(device_&&memory_)vkFreeMemory(device_,memory_,nullptr);buffer_=VK_NULL_HANDLE;memory_=VK_NULL_HANDLE;size_=0;device_=VK_NULL_HANDLE;physical_device_=VK_NULL_HANDLE;}
}
#endif
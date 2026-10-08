#include "rogue/rendering/vulkan/vulkan_buffer_copy.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
namespace rogue::rendering::vulkan {
bool record_buffer_copy_and_barrier(VkCommandBuffer cmd,VkBuffer src,VkBuffer dst,VkDeviceSize size){
 if(!cmd||!src||!dst||!size)return false;
 VkBufferCopy copy{0,0,size};vkCmdCopyBuffer(cmd,src,dst,1,&copy);
 VkBufferMemoryBarrier barrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
 barrier.srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
 barrier.dstAccessMask=VK_ACCESS_SHADER_READ_BIT;
 barrier.srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
 barrier.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
 barrier.buffer=dst;barrier.offset=0;barrier.size=size;
 vkCmdPipelineBarrier(cmd,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,0,0,nullptr,1,&barrier,0,nullptr);
 return true;
}
}
#endif
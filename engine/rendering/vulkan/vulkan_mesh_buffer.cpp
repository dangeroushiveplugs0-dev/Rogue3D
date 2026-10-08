#include "rogue/rendering/vulkan/vulkan_mesh_buffer.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <cstring>
namespace rogue::rendering::vulkan {
VulkanMeshBuffer::~VulkanMeshBuffer(){destroy();}
bool VulkanMeshBuffer::initialize(VkPhysicalDevice pd,VkDevice d,const rogue::mesh::Mesh& mesh){
 destroy();if(!pd||!d||!mesh.validate()||mesh.vertices().empty()||mesh.indices().empty())return false;
 physical_device_=pd;device_=d;
 const VkDeviceSize vb=mesh.vertices().size()*sizeof(rogue::mesh::Vertex),ib=mesh.indices().size()*sizeof(rogue::mesh::Index);
 if(!vertex_.initialize(pd,d,vb,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)||!index_.initialize(pd,d,ib,VK_BUFFER_USAGE_INDEX_BUFFER_BIT)){destroy();return false;}
 if(!staging_.initialize(pd,d,vb+ib,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT)){destroy();return false;}
 index_count_=static_cast<std::uint32_t>(mesh.index_count());return true;
}
bool VulkanMeshBuffer::update(VkCommandBuffer cmd,const rogue::mesh::Mesh& mesh){
 if(!is_initialized()||!cmd||!mesh.validate()||mesh.vertices().empty()||mesh.indices().empty())return false;
 const VkDeviceSize vb=mesh.vertices().size()*sizeof(rogue::mesh::Vertex),ib=mesh.indices().size()*sizeof(rogue::mesh::Index);if(vb>vertex_.size()||ib>index_.size()||vb+ib>staging_.size())return false;
 std::vector<std::uint8_t> bytes(static_cast<std::size_t>(vb+ib));std::memcpy(bytes.data(),mesh.vertices().data(),static_cast<std::size_t>(vb));std::memcpy(bytes.data()+vb,mesh.indices().data(),static_cast<std::size_t>(ib));
 if(!staging_.upload(bytes.data(),bytes.size()))return false;
 VkBufferCopy copies[2]={{0,0,vb},{vb,0,ib}};VkBuffer dst[2]={vertex_.buffer(),index_.buffer()};
 vkCmdCopyBuffer(cmd,staging_.buffer(),dst[0],1,&copies[0]);vkCmdCopyBuffer(cmd,staging_.buffer(),dst[1],1,&copies[1]);
 VkBufferMemoryBarrier barriers[2]{};
 for(int i=0;i<2;++i){barriers[i].sType=VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;barriers[i].srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;barriers[i].dstAccessMask=(i==0)?VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:VK_ACCESS_INDEX_READ_BIT;barriers[i].srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;barriers[i].dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;barriers[i].buffer=dst[i];barriers[i].size=VK_WHOLE_SIZE;}
 vkCmdPipelineBarrier(cmd,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,0,0,nullptr,2,barriers,0,nullptr);index_count_=static_cast<std::uint32_t>(mesh.index_count());return true;
}
void VulkanMeshBuffer::destroy()noexcept{staging_.destroy();index_.destroy();vertex_.destroy();index_count_=0;device_=VK_NULL_HANDLE;physical_device_=VK_NULL_HANDLE;}
}
#endif
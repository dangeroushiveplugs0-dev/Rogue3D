#include "rogue/rendering/vulkan/vulkan_morph_evaluator.hpp"
#if defined(ROGUE_ENABLE_VULKAN)
#include <cstring>
#include <limits>
namespace rogue::rendering::vulkan {
namespace {
struct SliceGpu { std::uint32_t morph_id,offset_bytes,element_count,storage_mode,precision; };
struct ActiveGpu { std::uint32_t morph_id; float weight; };
}
VulkanMorphEvaluator::~VulkanMorphEvaluator(){destroy();}
std::uint32_t VulkanMorphEvaluator::memory_type(std::uint32_t bits,VkMemoryPropertyFlags props) const noexcept{
 VkPhysicalDeviceMemoryProperties mp{}; vkGetPhysicalDeviceMemoryProperties(physical_device_,&mp);
 for(std::uint32_t i=0;i<mp.memoryTypeCount;++i) if((bits&(1u<<i))&&(mp.memoryTypes[i].propertyFlags&props)==props)return i;
 return std::numeric_limits<std::uint32_t>::max();
}
bool VulkanMorphEvaluator::make_buffer(VkDeviceSize size,VkBuffer* b,VkDeviceMemory* m){
 if(!size)return false; VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO}; bi.size=size; bi.usage=VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; bi.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
 if(vkCreateBuffer(device_,&bi,nullptr,b)!=VK_SUCCESS)return false; VkMemoryRequirements req{}; vkGetBufferMemoryRequirements(device_,*b,&req);
 auto mt=memory_type(req.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
 if(mt==std::numeric_limits<std::uint32_t>::max()){vkDestroyBuffer(device_,*b,nullptr);*b=VK_NULL_HANDLE;return false;}
 VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO}; ai.allocationSize=req.size; ai.memoryTypeIndex=mt;
 if(vkAllocateMemory(device_,&ai,nullptr,m)!=VK_SUCCESS){vkDestroyBuffer(device_,*b,nullptr);*b=VK_NULL_HANDLE;return false;}
 if(vkBindBufferMemory(device_,*b,*m,0)!=VK_SUCCESS){vkFreeMemory(device_,*m,nullptr);vkDestroyBuffer(device_,*b,nullptr);*m=VK_NULL_HANDLE;*b=VK_NULL_HANDLE;return false;}
 return true;
}
bool VulkanMorphEvaluator::upload(VkBuffer b,VkDeviceMemory m,VkDeviceSize size,const void* data,VkDeviceSize bytes){
 if(!b||!m||bytes>size)return false; void* p=nullptr; if(vkMapMemory(device_,m,0,bytes,0,&p)!=VK_SUCCESS)return false; std::memcpy(p,data,static_cast<std::size_t>(bytes)); vkUnmapMemory(device_,m); return true;
}
bool VulkanMorphEvaluator::initialize(VkPhysicalDevice pd,VkDevice dev,VkShaderModule shader,const MorphBuffer& source){
 destroy(); if(!pd||!dev||!shader||source.slices().empty()||source.active_morphs().empty()) return false;
 physical_device_=pd; device_=dev; shader_=shader;
 std::vector<SliceGpu> ss; ss.reserve(source.slices().size());
 for(const auto&s:source.slices()) ss.push_back({s.morph_id,(std::uint32_t)s.offset,s.element_count,(std::uint32_t)s.storage,(std::uint32_t)s.precision});
 std::vector<ActiveGpu> aa; aa.reserve(source.active_morphs().size());
 for(const auto&m:source.active_morphs()) aa.push_back({m.morph_id,m.weight});
 if(!make_buffer(ss.size()*sizeof(SliceGpu),&slices_,&slices_memory_)||!make_buffer(aa.size()*sizeof(ActiveGpu),&active_,&active_memory_)) {destroy();return false;}
 slices_size_=ss.size()*sizeof(SliceGpu); active_size_=aa.size()*sizeof(ActiveGpu);
 if(!upload(slices_,slices_memory_,slices_size_,ss.data(),slices_size_)||!upload(active_,active_memory_,active_size_,aa.data(),active_size_)){destroy();return false;}
 VkDescriptorSetLayoutBinding b[5]{};
 for(std::uint32_t i=0;i<5;++i){b[i].binding=i;b[i].descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;b[i].descriptorCount=1;b[i].stageFlags=VK_SHADER_STAGE_COMPUTE_BIT;}
 VkDescriptorSetLayoutCreateInfo li{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO}; li.bindingCount=5; li.pBindings=b;
 if(vkCreateDescriptorSetLayout(device_,&li,nullptr,&layout_)!=VK_SUCCESS){destroy();return false;}
 VkPushConstantRange pc{}; pc.stageFlags=VK_SHADER_STAGE_COMPUTE_BIT; pc.offset=0; pc.size=12;
 VkPipelineLayoutCreateInfo pli{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO}; pli.setLayoutCount=1; pli.pSetLayouts=&layout_; pli.pushConstantRangeCount=1; pli.pPushConstantRanges=&pc;
 if(vkCreatePipelineLayout(device_,&pli,nullptr,&pipeline_layout_)!=VK_SUCCESS){destroy();return false;}
 VkComputePipelineCreateInfo ci{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO}; VkPipelineShaderStageCreateInfo stage{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}; stage.stage=VK_SHADER_STAGE_COMPUTE_BIT;stage.module=shader_;stage.pName="main";ci.stage=stage;ci.layout=pipeline_layout_;
 if(vkCreateComputePipelines(device_,VK_NULL_HANDLE,1,&ci,nullptr,&pipeline_)!=VK_SUCCESS){destroy();return false;}
 VkDescriptorPoolSize ps{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,5}; VkDescriptorPoolCreateInfo pi{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};pi.maxSets=1;pi.poolSizeCount=1;pi.pPoolSizes=&ps;
 if(vkCreateDescriptorPool(device_,&pi,nullptr,&pool_)!=VK_SUCCESS){destroy();return false;}
 VkDescriptorSetAllocateInfo da{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};da.descriptorPool=pool_;da.descriptorSetCount=1;da.pSetLayouts=&layout_;
 if(vkAllocateDescriptorSets(device_,&da,&set_)!=VK_SUCCESS){destroy();return false;}
 slice_count_=static_cast<std::uint32_t>(ss.size()); active_count_=static_cast<std::uint32_t>(aa.size()); return true;
}
bool VulkanMorphEvaluator::update_morphs(const MorphBuffer& source){
 if(!is_initialized())return false; if(source.slices().size()*sizeof(SliceGpu)>slices_size_||source.active_morphs().size()*sizeof(ActiveGpu)>active_size_)return false;
 std::vector<SliceGpu> ss;for(const auto&s:source.slices())ss.push_back({s.morph_id,(std::uint32_t)s.offset,s.element_count,(std::uint32_t)s.storage,(std::uint32_t)s.precision});
 std::vector<ActiveGpu> aa;for(const auto&m:source.active_morphs())aa.push_back({m.morph_id,m.weight});
 if(!upload(slices_,slices_memory_,slices_size_,ss.data(),ss.size()*sizeof(SliceGpu))||!upload(active_,active_memory_,active_size_,aa.data(),aa.size()*sizeof(ActiveGpu)))return false;
 slice_count_=static_cast<std::uint32_t>(ss.size());active_count_=static_cast<std::uint32_t>(aa.size());return true;
}
bool VulkanMorphEvaluator::dispatch(VkCommandBuffer cmd,VkBuffer base,VkBuffer output,std::uint32_t vertex_count){
 if(!is_initialized()||!cmd||!base||!output||!vertex_count)return false;
 VkDescriptorBufferInfo infos[5]{}; infos[0]={base,0,VK_WHOLE_SIZE};infos[1]={output,0,VK_WHOLE_SIZE};infos[2]={VK_NULL_HANDLE,0,VK_WHOLE_SIZE};infos[3]={slices_,0,slices_size_};infos[4]={active_,0,active_size_};
 VkWriteDescriptorSet writes[5]{};for(std::uint32_t i=0;i<5;++i){writes[i]={VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};writes[i].dstSet=set_;writes[i].dstBinding=i;writes[i].descriptorCount=1;writes[i].descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;writes[i].pBufferInfo=&infos[i];}
 vkUpdateDescriptorSets(device_,4,writes+0,0,nullptr);
 vkCmdBindPipeline(cmd,VK_PIPELINE_BIND_POINT_COMPUTE,pipeline_);vkCmdBindDescriptorSets(cmd,VK_PIPELINE_BIND_POINT_COMPUTE,pipeline_layout_,0,1,&set_,0,nullptr);
 std::uint32_t pc[3]={vertex_count,slice_count_,active_count_};vkCmdPushConstants(cmd,pipeline_layout_,VK_SHADER_STAGE_COMPUTE_BIT,0,sizeof(pc),pc);
 vkCmdDispatch(cmd,(vertex_count+63u)/64u,1,1);return true;
}
void VulkanMorphEvaluator::destroy()noexcept{
 if(device_){if(pipeline_)vkDestroyPipeline(device_,pipeline_,nullptr);if(pipeline_layout_)vkDestroyPipelineLayout(device_,pipeline_layout_,nullptr);if(pool_)vkDestroyDescriptorPool(device_,pool_,nullptr);if(layout_)vkDestroyDescriptorSetLayout(device_,layout_,nullptr);if(slices_)vkDestroyBuffer(device_,slices_,nullptr);if(slices_memory_)vkFreeMemory(device_,slices_memory_,nullptr);if(active_)vkDestroyBuffer(device_,active_,nullptr);if(active_memory_)vkFreeMemory(device_,active_memory_,nullptr);}
 pipeline_=VK_NULL_HANDLE;pipeline_layout_=VK_NULL_HANDLE;pool_=VK_NULL_HANDLE;layout_=VK_NULL_HANDLE;slices_=VK_NULL_HANDLE;slices_memory_=VK_NULL_HANDLE;active_=VK_NULL_HANDLE;active_memory_=VK_NULL_HANDLE;slices_size_=active_size_=0;slice_count_=active_count_=0;device_=VK_NULL_HANDLE;physical_device_=VK_NULL_HANDLE;shader_=VK_NULL_HANDLE;
}
}
#endif
#pragma once
#include "common.hpp"

namespace ezvk {
struct CommandPool {
  VkCommandPool cmdPool;

  void create(VkDevice device, VkCommandPoolCreateFlags flag, u32 queueIndex);
  void destroy(VkDevice device);

  EZVK_CONVERT_OP(VkCommandPool, cmdPool);
  EZVK_ADDRESS_OP(VkCommandPool, cmdPool);
};
struct CommandBuffer {
  VkCommandBuffer cmdBuffer;

  void alloc(VkDevice device, VkCommandPool cmdPool,
             VkCommandBufferLevel level);
  void free(VkDevice device, VkCommandPool cmdPool);

  VkResult           reset(VkCommandBufferResetFlags flag);
  void           beginWithOneTimeSubmit();
  void           begin(VkCommandBufferUsageFlags             flag,
                       const VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);
  void           end();
  CommandBuffer& beginRenderPass(VkRenderPassBeginInfo* pRenderPassBI,
                                 VkSubpassContents      contents);
  CommandBuffer& endRenderPass();

  CommandBuffer& bindVertexBuffer(VkBuffer buffer);
  CommandBuffer& bindVertexBuffers(u32 firstBinding, u32 bindingCount,
                                   const VkBuffer*     pBuffers,
                                   const VkDeviceSize* pOffsets);

  CommandBuffer& bindIndexBuffer(VkBuffer buffer, VkIndexType indexType,
                                 size_t offset = 0);
  CommandBuffer& bindPipeline(VkPipelineBindPoint bindPoint,
                              VkPipeline          pipeline);
  CommandBuffer& bindPipelineGraphic(VkPipeline pipeline);
  CommandBuffer& draw(u32 vertexCount, u32 instanceCount, u32 firstVertex,
                      u32 firstInstance);
  CommandBuffer& drawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex,
                             u32 vertexOffset, u32 firstInstance);
  CommandBuffer& copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                            u32 regionCount, const VkBufferCopy* pRegions);

  CommandBuffer& dispatch(u32 groupX, u32 groupY, u32 groupZ) {
    vkCmdDispatch(cmdBuffer, groupX, groupY, groupZ);
    return *this;
  }

  CommandBuffer& copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage,
                                   VkImageLayout            dstImageLayout,
                                   u32                      regionCount,
                                   const VkBufferImageCopy* pRegions) {
    vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, dstImageLayout,
                           regionCount, pRegions);
    return *this;
  }

  CommandBuffer& copyImageToBuffer(VkImage       srcImage,
                                   VkImageLayout srcImageLayout,
                                   VkBuffer dstBuffer, u32 regionCount,
                                   const VkBufferImageCopy* pRegions) {
    vkCmdCopyImageToBuffer(cmdBuffer, srcImage, srcImageLayout, dstBuffer,
                           regionCount, pRegions);
    return *this;
  }

  CommandBuffer& bindDescriptorSet(VkPipelineBindPoint bindPoint,
                                   VkPipelineLayout layout, u32 firstSet,
                                   u32              setCount,
                                   VkDescriptorSet* pDescriptorSets,
                                   u32              dynamicOffsetCount,
                                   u32*             pDynamicOffsets);
  CommandBuffer& bindDescriptorSetNoDynamic(VkPipelineBindPoint bindPoint,
                                            VkPipelineLayout    layout,
                                            u32 firstSet, u32 setCount,
                                            VkDescriptorSet* pDescriptorSets);

  CommandBuffer& pipelineBufferBarrier(
      VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
      VkDependencyFlags dependencyFlags, u32 bufferMemoryBarrierCount,
      const VkBufferMemoryBarrier* pBufferMemoryBarriers) {
    return pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 0,
                           nullptr, bufferMemoryBarrierCount,
                           pBufferMemoryBarriers, 0, nullptr);
  }

  CommandBuffer& pipelineMemoryBarrier(VkPipelineStageFlags srcStageMask,
                                       VkPipelineStageFlags dstStageMask,
                                       VkDependencyFlags    dependencyFlags,
                                       u32                  memoryBarrierCount,
                                       const VkMemoryBarrier* pMemoryBarriers) {
    return pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags,
                           memoryBarrierCount, pMemoryBarriers, 0, nullptr, 0,
                           nullptr);
  }

  CommandBuffer& pipelineImageBarrier(
      VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
      VkDependencyFlags dependencyFlags, u32 imageMemoryBarrierCount,
      const VkImageMemoryBarrier* pImageMemoryBarriers) {
    return pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 0,
                           nullptr, 0, nullptr, imageMemoryBarrierCount,
                           pImageMemoryBarriers);
  }

  CommandBuffer& pipelineBarrier(
      VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
      VkDependencyFlags dependencyFlags, u32 memoryBarrierCount,
      const VkMemoryBarrier* pMemoryBarriers, u32 bufferMemoryBarrierCount,
      const VkBufferMemoryBarrier* pBufferMemoryBarriers,
      u32                          imageMemoryBarrierCount,
      const VkImageMemoryBarrier*  pImageMemoryBarriers) {
    vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, dependencyFlags,
                         memoryBarrierCount, pMemoryBarriers,
                         bufferMemoryBarrierCount, pBufferMemoryBarriers,
                         imageMemoryBarrierCount, pImageMemoryBarriers);
    return *this;
  }

  EZVK_CONVERT_OP(VkCommandBuffer, cmdBuffer);
  EZVK_ADDRESS_OP(VkCommandBuffer, cmdBuffer);
};
} // namespace ezvk
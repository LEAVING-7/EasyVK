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

  void reset(VkCommandBufferResetFlags flag);
  void beginWithOneTimeSubmit();
  void begin(VkCommandBufferUsageFlags             flag,
             const VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);
  void end();
  void beginRenderPass(VkRenderPassBeginInfo* pRenderPassBI,
                       VkSubpassContents      contents);
  void endRenderPass();

  void bindVertexBuffer(VkBuffer buffer);
  void bindVertexBuffers(u32 firstBinding, u32 bindingCount,
                         const VkBuffer*     pBuffers,
                         const VkDeviceSize* pOffsets);

  void bindIndexBuffer(VkBuffer buffer, VkIndexType indexType,
                       size_t offset = 0);
  void bindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline);
  void bindPipelineGraphic(VkPipeline pipeline);
  void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex,
            u32 firstInstance);
  void drawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex,
                   u32 vertexOffset, u32 firstInstance);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, u32 regionCount,
                  const VkBufferCopy* pRegions);

  void dispatch(u32 groupX, u32 groupY, u32 groupZ) {
    vkCmdDispatch(cmdBuffer, groupX, groupY, groupZ);
  }

  void copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage,
                         VkImageLayout dstImageLayout, u32 regionCount,
                         const VkBufferImageCopy* pRegions) {
    vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, dstImageLayout,
                           regionCount, pRegions);
  }

  void copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout,
                         VkBuffer dstBuffer, u32 regionCount,
                         const VkBufferImageCopy* pRegions) {
    vkCmdCopyImageToBuffer(cmdBuffer, srcImage, srcImageLayout, dstBuffer,
                           regionCount, pRegions);
  }

  void bindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout layout,
                         u32 firstSet, u32 setCount,
                         VkDescriptorSet* pDescriptorSets,
                         u32 dynamicOffsetCount, u32* pDynamicOffsets);
  void bindDescriptorSetNoDynamic(VkPipelineBindPoint bindPoint,
                                  VkPipelineLayout layout, u32 firstSet,
                                  u32              setCount,
                                  VkDescriptorSet* pDescriptorSets);

  void pipelineBufferBarrier(
      VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
      VkDependencyFlags dependencyFlags, u32 bufferMemoryBarrierCount,
      const VkBufferMemoryBarrier* pBufferMemoryBarriers) {
    return pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 0,
                           nullptr, bufferMemoryBarrierCount,
                           pBufferMemoryBarriers, 0, nullptr);
  }

  void pipelineMemoryBarrier(VkPipelineStageFlags   srcStageMask,
                             VkPipelineStageFlags   dstStageMask,
                             VkDependencyFlags      dependencyFlags,
                             u32                    memoryBarrierCount,
                             const VkMemoryBarrier* pMemoryBarriers) {
    return pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags,
                           memoryBarrierCount, pMemoryBarriers, 0, nullptr, 0,
                           nullptr);
  }

  void pipelineImageBarrier(VkPipelineStageFlags        srcStageMask,
                            VkPipelineStageFlags        dstStageMask,
                            VkDependencyFlags           dependencyFlags,
                            u32                         imageMemoryBarrierCount,
                            const VkImageMemoryBarrier* pImageMemoryBarriers) {
    return pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, 0,
                           nullptr, 0, nullptr, imageMemoryBarrierCount,
                           pImageMemoryBarriers);
  }

  void pipelineBarrier(VkPipelineStageFlags         srcStageMask,
                       VkPipelineStageFlags         dstStageMask,
                       VkDependencyFlags            dependencyFlags,
                       u32                          memoryBarrierCount,
                       const VkMemoryBarrier*       pMemoryBarriers,
                       u32                          bufferMemoryBarrierCount,
                       const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                       u32                          imageMemoryBarrierCount,
                       const VkImageMemoryBarrier*  pImageMemoryBarriers) {
    vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, dependencyFlags,
                         memoryBarrierCount, pMemoryBarriers,
                         bufferMemoryBarrierCount, pBufferMemoryBarriers,
                         imageMemoryBarrierCount, pImageMemoryBarriers);
  }

  EZVK_CONVERT_OP(VkCommandBuffer, cmdBuffer);
  EZVK_ADDRESS_OP(VkCommandBuffer, cmdBuffer);
};
} // namespace ezvk
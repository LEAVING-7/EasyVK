#pragma once
#include "common.hpp"

namespace ezvk {
struct Fence {
  VkFence fence;

  void     create(VkDevice device, VkFenceCreateFlags flag);
  void     createSignaled(VkDevice device);
  void     destroy(VkDevice device);
  VkResult reset(VkDevice device);

  EZVK_CONVERT_OP(Fence, fence);
  EZVK_ADDRESS_OP(Fence, fence);
};
struct Semaphore {
  VkSemaphore semaphore;

  void create(VkDevice device);
  void destroy(VkDevice device);

  EZVK_CONVERT_OP(Semaphore, semaphore);
  EZVK_ADDRESS_OP(Semaphore, semaphore);
};
} // namespace ezvk
#pragma once
#include "common.hpp"

namespace ezvk {
struct Fence {
  VkFence fence;

  void     create(VkDevice device, VkFenceCreateFlags flag);
  void     createSignaled(VkDevice device);
  void     destroy(VkDevice device);
  VkResult reset(VkDevice device);

  EZVK_CONVERT_OP(VkFence, fence);
  EZVK_ADDRESS_OP(VkFence, fence);
};
struct Semaphore {
  VkSemaphore semaphore;

  void create(VkDevice device);
  void destroy(VkDevice device);

  EZVK_CONVERT_OP(VkSemaphore, semaphore);
  EZVK_ADDRESS_OP(VkSemaphore, semaphore);
};
} // namespace ezvk
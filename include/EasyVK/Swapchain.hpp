#pragma once
#include "common.hpp"

namespace ezvk {
class Device;
class Swapchain {
public:
  vkb::Swapchain m_swapchain;

  std::vector<VkImage>     m_images;
  std::vector<VkImageView> m_imageViews;

  void create(vkb::Device* device, VkSurfaceKHR surface, u32 width, u32 height);
  void destroy();
  u32  getImageCount() {
     return m_swapchain.image_count;
  }

  EZVK_CONVERT_OP(VkSwapchainKHR, m_swapchain);
};
} // namespace ezvk
#pragma once
#include "common.hpp"

namespace ezvk {
class Device {
public:
  vkb::Device         m_device;
  vkb::PhysicalDevice m_gpu;

  void create(vkb::PhysicalDevice& gpu, std::vector<ccstr>&& layers,
              std::vector<ccstr>&& extensions);
  void destroy();

  EZVK_CONVERT_OP(VkDevice, m_device);
};

} // namespace ezvk
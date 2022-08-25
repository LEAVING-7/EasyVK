#pragma once
#include "common.hpp"

namespace ezvk {
class Device {
private:
  using SelectFunc = void(vkb::PhysicalDeviceSelector&);
  using BuildFunc  = void(vkb::DeviceBuilder&);

public:
  vkb::Device         m_device;
  vkb::PhysicalDevice m_gpu;

  void create(vkb::PhysicalDevice& gpu);
  void create(
      vkb::Instance& instance, SelectFunc selectGPU,
      VkSurfaceKHR surface   = VK_NULL_HANDLE,
      BuildFunc    buildFunc = [](vkb::DeviceBuilder&) {});
  void destroy();

  EZVK_CONVERT_OP(VkDevice, m_device);
};

} // namespace ezvk
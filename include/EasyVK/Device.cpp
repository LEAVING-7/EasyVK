#include "Device.hpp"

namespace ezvk {
void Device::create(vkb::PhysicalDevice& gpu) {
  vkb::DeviceBuilder builder{gpu};

  auto result = builder.build();
  assert(result.has_value());
  m_device = std::move(result.value());
  m_gpu    = std::move(gpu);
}
void Device::create(vkb::Instance& instance, SelectFunc selectGPU,
                    VkSurfaceKHR surface, BuildFunc buildFunc) {
  vkb::PhysicalDeviceSelector selector{instance, surface};
  selectGPU(selector);
  auto physicalRes = selector.select();
  assert(physicalRes.has_value());
  m_gpu = std::move(physicalRes.value());

  vkb::DeviceBuilder builder{m_gpu};
  buildFunc(builder);
  auto deviceRes = builder.build();
  assert(deviceRes.has_value());
  m_device = std::move(deviceRes.value());
}
void Device::destroy() {
  vkb::destroy_device(m_device);
}
} // namespace ezvk
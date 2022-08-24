#include "Device.hpp"

namespace ezvk {
void Device::create(vkb::PhysicalDevice& gpu) {
  vkb::DeviceBuilder builder{gpu};

  auto result = builder.build();
  assert(result.has_value());
  m_device = std::move(result.value());
  m_gpu    = std::move(gpu);
}
void Device::destroy() {
  vkb::destroy_device(m_device);
}
} // namespace ezvk
#include "Device.hpp"

namespace ezvk {
void Device::create(vkb::PhysicalDevice& gpu, std::vector<ccstr>& layers,
                    std::vector<ccstr>& extensions) {
  vkb::DeviceBuilder builder{gpu};

  auto result = builder.build();
  assert(result.has_value());
  m_device = std::move(result.value());
  m_gpu    = std::move(gpu);

  VkImageFormatProperties prop;
  auto                    res = vkGetPhysicalDeviceImageFormatProperties(
                         m_gpu.physical_device, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_TYPE_2D,
                         VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 0, &prop);
  assert(res != VK_ERROR_FORMAT_NOT_SUPPORTED);
}
void Device::destroy() {
  vkb::destroy_device(m_device);
}
} // namespace ezvk
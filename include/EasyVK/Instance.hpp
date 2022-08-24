#pragma once
#include "common.hpp"

namespace ezvk {
class Instance {
private:
  vkb::Instance m_instance;

private:
  VkDebugUtilsMessengerEXT             m_messenger;
  PFN_vkDebugUtilsMessengerCallbackEXT m_fpDebugMessenger{nullptr};
  bool                                 m_enableValidationLayer;

public:
  void setDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback) {
    m_fpDebugMessenger = callback;
  };
  void create(std::vector<ccstr>&& layers, std::vector<ccstr>&& extensions,
              ccstr appName, bool enableValidationLayer = true) {
    return create(layers, extensions, appName, enableValidationLayer);
  }
  void create(std::vector<ccstr>& layers, std::vector<ccstr>& extensions,
              ccstr appName, bool enableValidationLayer = true);
  void destroy();

  EZVK_CONVERT_OP(VkInstance, m_instance);
  EZVK_CONVERT_OP(vkb::Instance, m_instance);
};
} // namespace ezvk
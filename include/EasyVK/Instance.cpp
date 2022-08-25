#include "Instance.hpp"

namespace ezvk {
void Instance::create(std::vector<ccstr>& layers,
                      std::vector<ccstr>& extensions, ccstr appName,
                      bool enableValidationLayer) {
  m_enableValidationLayer = enableValidationLayer;
  vkb::InstanceBuilder builder{};
  builder.set_app_name(appName)
      .set_engine_name(appName)
      .set_app_version(1)
      .set_engine_version(1)
      .require_api_version(1, 2);

  for (ccstr layerName : layers) {
    builder.enable_layer(layerName);
  }
  for (ccstr extensionName : extensions) {
    builder.enable_extension(extensionName);
  }
  if (enableValidationLayer) {
    builder.enable_validation_layers();
    if (!m_fpDebugMessenger)
      builder.use_default_debug_messenger();
    else
      builder.set_debug_callback(m_fpDebugMessenger);
  }

  auto result = builder.build();
  assert(result.has_value());
  m_instance = result.value();
}
void Instance::create(std::vector<ccstr>& layers,
                      std::vector<ccstr>& extensions, ProcFuncType processFunc,
                      bool enableValidationLayer) {
  vkb::InstanceBuilder builder{};
  processFunc(builder);
  for (ccstr layerName : layers) {
    builder.enable_layer(layerName);
  }
  for (ccstr extensionName : extensions) {
    builder.enable_extension(extensionName);
  }
  if (enableValidationLayer) {
    builder.enable_validation_layers();
    if (!m_fpDebugMessenger)
      builder.use_default_debug_messenger();
    else
      builder.set_debug_callback(m_fpDebugMessenger);
  }
  auto instanceResult = builder.build();
  assert(instanceResult.has_value());
  m_instance = std::move(instanceResult.value());
}

void Instance::destroy() {
  vkb::destroy_instance(m_instance);
}
} // namespace ezvk
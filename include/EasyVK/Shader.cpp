#include "Shader.hpp"
namespace ezvk {

void Shader::create(VkDevice device, VkShaderStageFlagBits stage, u32* code,
                    size_t size, ccstr entryName,
                    const VkSpecializationInfo* pSpecializationInfo) {
  assert(size % 4 == 0);
  m_stage      = stage;
  m_shaderInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = m_stage,
      .pName = entryName,
      .pSpecializationInfo = pSpecializationInfo,
  };

  VkShaderModuleCreateInfo moduleCreateInfo{
      .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext    = nullptr,
      .flags    = 0,
      .codeSize = size,
      .pCode    = code,
  };

  VkResult result;
  result = vkCreateShaderModule(device, &moduleCreateInfo, nullptr,
                                &m_shaderInfo.module);
  assert(result == VK_SUCCESS);
};

void Shader::destroy(VkDevice device) {
  vkDestroyShaderModule(device, m_shaderInfo.module, nullptr);
}
} // namespace ezvk
#pragma once
#include "common.hpp"

namespace ezvk {
struct PipelineLayout {
  VkPipelineLayout layout;

  VkResult create(VkDevice                             device,
                  std::vector<VkDescriptorSetLayout>&& setLayout,
                  std::vector<VkPushConstantRange>&&   pushConstants) {
    return create(device, setLayout, pushConstants);
  }
  VkResult create(VkDevice                            device,
                  std::vector<VkDescriptorSetLayout>& setLayout,
                  std::vector<VkPushConstantRange>&   pushConstant) {
    VkPipelineLayoutCreateInfo CI{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = (u32)setLayout.size(),
        .pSetLayouts            = setLayout.data(),
        .pushConstantRangeCount = (u32)pushConstant.size(),
        .pPushConstantRanges    = pushConstant.data(),
    };
    return vkCreatePipelineLayout(device, &CI, nullptr, &layout);
  }

  void destroy(VkDevice device) {
    return vkDestroyPipelineLayout(device, layout, nullptr);
  }
  // namespace ezvk

  EZVK_CONVERT_OP(VkPipelineLayout, layout);
};

struct GraphicPipelineBuilder {

  std::vector<VkPipelineShaderStageCreateInfo> shaders;
  std::vector<VkDynamicState>                  dynamicStates;

  std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
  std::vector<VkVertexInputBindingDescription>   VertexInputBindings;
  VkPipelineInputAssemblyStateCreateInfo         inputAssembly;

  std::vector<VkViewport> viewports;
  std::vector<VkRect2D>   scissors;

  VkPipelineRasterizationStateCreateInfo rasterizer;
  VkPipelineColorBlendAttachmentState    colorBlendAttachment;

  VkPipelineTessellationStateCreateInfo tessellation;
  VkPipelineMultisampleStateCreateInfo  multisampling;
  VkPipelineDepthStencilStateCreateInfo depthStencil;

public:
  VkPipeline build(VkDevice device, VkRenderPass pass, VkPipelineLayout layout,
                   bool            enableTessellation = false,
                   VkPipelineCache cache              = VK_NULL_HANDLE);

  std::pair<VkPipeline, VkPipelineCache>
  buildWithCache(VkDevice device, VkRenderPass pass, VkPipelineLayout layout,
                 VkPipelineCache cache  = VK_NULL_HANDLE,
                 size_t initialDataSize = 0, void* pInitialData = nullptr);

public:
  GraphicPipelineBuilder&
  setShader(std::vector<VkPipelineShaderStageCreateInfo>&& shaders) {
    this->shaders = std::move(shaders);
    return *this;
  }

  GraphicPipelineBuilder&
  setShader(std::vector<VkPipelineShaderStageCreateInfo>& shaders) {
    this->shaders = shaders;
    return *this;
  }

  GraphicPipelineBuilder& noColorBlend(VkColorComponentFlags colorWriteMask);

  GraphicPipelineBuilder&
  setDynamic(std::vector<VkDynamicState>& dynamicStates) {
    this->dynamicStates = dynamicStates;
    return *this;
  }

  GraphicPipelineBuilder&
  setDynamic(std::vector<VkDynamicState>&& dynamicState) {
    this->dynamicStates = std::move(dynamicState);
    return *this;
  }

  GraphicPipelineBuilder& setVertexAttributes(
      std::vector<VkVertexInputAttributeDescription>& attributes) {
    this->vertexInputAttributes = attributes;
    return *this;
  }

  GraphicPipelineBuilder& setVertexAttributes(
      std::vector<VkVertexInputAttributeDescription>&& attributes) {
    this->vertexInputAttributes = std::move(attributes);
    return *this;
  }

  GraphicPipelineBuilder&
  setVertexBindings(std::vector<VkVertexInputBindingDescription>& bindings) {
    this->VertexInputBindings = bindings;
    return *this;
  }

  GraphicPipelineBuilder&
  setVertexBindings(std::vector<VkVertexInputBindingDescription>&& bindings) {
    this->VertexInputBindings = std::move(bindings);
    return *this;
  }

  GraphicPipelineBuilder& setInputAssembly(VkPrimitiveTopology topology,
                                           VkBool32 primitiveRestartEnable);

  GraphicPipelineBuilder&
  setRasterization(VkBool32 depthClampEnable, VkBool32 rasterizerDiscardEnable,
                   VkPolygonMode polygonMode, VkCullModeFlags cullMode,
                   VkFrontFace frontFace, VkBool32 depthBiasEnable,
                   float depthBiasConstantFactor, float depthBiasClamp,
                   float depthBiasSlopeFactor, float lineWidth);

  GraphicPipelineBuilder&
  setMultisample(VkSampleCountFlagBits rasterizationSamples,
                 VkBool32 sampleShadingEnable, float minSampleShadin,
                 const VkSampleMask* pSampleMask,
                 VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnabl);

  GraphicPipelineBuilder& setColorBlendAttachment(
      VkBool32 blendEnable, VkBlendFactor srcColorBlendFactor,
      VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp,
      VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor,
      VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask);

  GraphicPipelineBuilder& setTessellation(uint32_t patchControlPoints);

  GraphicPipelineBuilder&
  setDepthStencil(VkBool32 depthTestEnable, VkBool32 depthWriteEnable,
                  VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnable,
                  VkBool32 stencilTestEnable, VkStencilOpState front,
                  VkStencilOpState back, float minDepthBounds,
                  float maxDepthBounds);
  GraphicPipelineBuilder& setDeepNoStencil(VkBool32    depthWriteEnable,
                                           VkCompareOp depthCompareOp,
                                           VkBool32    depthBoundsTestEnable,
                                           float       minDepthBounds,
                                           float       maxDepthBounds);

  GraphicPipelineBuilder&
  setViewPortAndScissor(std::vector<VkViewport>& viewports,
                        std::vector<VkRect2D>&   scissors) {
    this->viewports = viewports;
    this->scissors  = scissors;
    return *this;
  }

  GraphicPipelineBuilder&
  setViewPortAndScissor(std::vector<VkViewport>&& viewports,
                        std::vector<VkRect2D>&&   scissors) {
    this->viewports = std::move(viewports);
    this->scissors  = std::move(scissors);
    return *this;
  }

  GraphicPipelineBuilder& setViewPortAndScissorCount(size_t viewportCount,
                                                     size_t scissorCount) {
    this->viewports.resize(viewportCount);
    this->scissors.resize(scissorCount);
    return *this;
  }

  GraphicPipelineBuilder& setLineWidth(float lineWidth) {
    this->rasterizer.lineWidth = lineWidth;
    return *this;
  }
};

struct ComputePipelineBuilder {
  std::vector<VkComputePipelineCreateInfo> computeCreateInfo;

  ComputePipelineBuilder& add(VkPipelineCreateFlags           flags,
                              VkPipelineShaderStageCreateInfo stage,
                              VkPipelineLayout                layout,
                              VkPipeline basePipelineHandle = VK_NULL_HANDLE,
                              i32        basePipelineIndex  = 0) {
    VkComputePipelineCreateInfo CI{
        .sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = flags,
        .stage              = stage,
        .layout             = layout,
        .basePipelineHandle = basePipelineHandle,
        .basePipelineIndex  = basePipelineIndex,
    };
    computeCreateInfo.push_back(CI);
    return *this;
  }

  std::vector<VkPipeline> build(VkDevice        device,
                                VkPipelineCache pipelineCache) {
    std::vector<VkPipeline> ret(computeCreateInfo.size());

    auto res = vkCreateComputePipelines(
        device, pipelineCache, (u32)computeCreateInfo.size(),
        computeCreateInfo.data(), nullptr, ret.data());
    assert(res == VK_SUCCESS);
    return ret;
  }
};
} // namespace ezvk
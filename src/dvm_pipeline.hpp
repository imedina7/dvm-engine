#pragma once
#include "dvm_device.hpp"
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace dvm
{
struct PipelineConfigInfo
{
  PipelineConfigInfo() = default;
  PipelineConfigInfo(const PipelineConfigInfo&) = delete;
  PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

  std::vector<VkVertexInputBindingDescription> bindingDescriptions {};
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions {};
  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;

  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class DvmPipeline
{
public:
  enum class PipelineType
  {
    GRAPHICS = 0,
    COMPUTE,
    TOTAL_TYPES
  };

public:
  DvmPipeline(DvmDevice& device,
              const std::vector<std::string>& shaderFilepaths,
              const PipelineConfigInfo& configInfo,
              const PipelineType& pipelineType);
  ~DvmPipeline();

  DvmPipeline(const DvmPipeline&) = delete;
  DvmPipeline& operator=(const DvmPipeline&) = delete;

  void bind(VkCommandBuffer commandBuffer);

  static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

  static void enableAlphaBlending(PipelineConfigInfo& configInfo);

private:
  static std::vector<char> readFile(const std::string& filepath);

  void createGraphicsPipeline(const std::string& vertFilepath,
                              const std::string& fragFilepath,
                              const PipelineConfigInfo& configInfo);

  void createComputePipeline(const std::string& shaderFilepath,
                             const PipelineConfigInfo& configInfo);

  void createShaderModule(const std::vector<char>& code,
                          VkShaderModule* shaderModule);

  DvmDevice& dvmDevice;
  VkPipeline graphicsPipeline;
  VkPipeline computePipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
  VkShaderModule computeShaderModule;
  PipelineType pipelineType;
};
}  // namespace dvm
#include "simple_render_system.hpp"
#include "dvm_swap_chain.hpp"
#include <array>
#include <entt.hpp>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

namespace dvm
{
struct SimplePushConstantData
{
  glm::mat4 modelMatrix {1.0f};
  glm::mat4 normalMatrix {1.0f};
};

SimpleRenderSystem::SimpleRenderSystem(DvmDevice& device,
                                       VkRenderPass renderPass,
                                       VkDescriptorSetLayout globalSetLayout)
    : dvmDevice {device}
{
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}
SimpleRenderSystem::~SimpleRenderSystem()
{
  vkDestroyPipelineLayout(dvmDevice.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout(
    VkDescriptorSetLayout globalSetLayout)
{
  VkPushConstantRange pushConstantRange {
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(SimplePushConstantData)};

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts {globalSetLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(
          dvmDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
{
  PipelineConfigInfo pipelineConfig {};
  DvmPipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  std::vector<std::string> shaders = {"shaders/simple_shader.vert.spv",
                                      "shaders/simple_shader.frag.spv"};

  dvmPipeline = std::make_unique<DvmPipeline>(
      dvmDevice, shaders, pipelineConfig, DvmPipeline::PipelineType::GRAPHICS);
}

void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo)
{
  dvmPipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout,
                          0,
                          1,
                          &frameInfo.globalDescriptorSet,
                          0,
                          nullptr);

  auto view = frameInfo.registry.view<Renderable, TransformComponent>();

  for (auto&& [entity, model, transform] : view.each()) {
    SimplePushConstantData push {};

    push.normalMatrix = transform.normalMatrix();
    push.modelMatrix = transform.mat4();

    vkCmdPushConstants(
        frameInfo.commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &push);

    model.model->bind(frameInfo.commandBuffer);
    model.model->draw(frameInfo.commandBuffer);
  }
}
}  // namespace dvm
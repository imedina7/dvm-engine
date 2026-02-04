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

SimpleRenderSystem::SimpleRenderSystem(
    DvmDevice& device,
    DvmWindow& window)
    : dvmDevice {device}, dvmWindow{window}
{
  createDescriptorSetLayouts();
  dvmRenderer = std::make_unique<DvmRenderer>(dvmWindow, dvmDevice);
  VkRenderPass renderPass = dvmRenderer->getSwapChainRenderPass();
  std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {globalSetLayout->getDescriptorSetLayout()};
  createPipelineLayout(
      descriptorSetLayouts);
  createPipeline(renderPass);
  createBuffers();
  createDescriptorPool();
  createDescriptorSets();
}
SimpleRenderSystem::~SimpleRenderSystem()
{
  vkDestroyPipelineLayout(dvmDevice.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout(
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
  VkPushConstantRange pushConstantRange {
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(SimplePushConstantData)};

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
  dvmPipeline = std::make_unique<DvmPipeline>(dvmDevice,
                                              "shaders/simple_shader.vert.spv",
                                              "shaders/simple_shader.frag.spv",
                                              pipelineConfig);
}

void SimpleRenderSystem::createDescriptorPool() {
   globalPool = DvmDescriptorPool::Builder(dvmDevice)
                    .setMaxSets(DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                 DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                    .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                 DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                    .build();
 }
void SimpleRenderSystem::createDescriptorSetLayouts() {
  globalSetLayout =
      DvmDescriptorSetLayout::Builder(dvmDevice)
          .addBinding(0,
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                      VK_SHADER_STAGE_ALL_GRAPHICS)
          .addBinding(1,
                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_ALL_GRAPHICS)
          .build();
}
void SimpleRenderSystem::createDescriptorSets() {
  VkDescriptorImageInfo imageInfo = texture.getDescriptorImageInfo();

  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers.at(i)->descriptorInfo();
    DvmDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .writeImage(1, &imageInfo)
        .build(globalDescriptorSets[i]);
  }
}
void SimpleRenderSystem::createBuffers() {
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<DvmBuffer>(
        dvmDevice,
        sizeof(GlobalUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        dvmDevice.properties.limits.minUniformBufferOffsetAlignment);
    uboBuffers[i]->map();
  }

  for(int i = 0; i < materialBuffers.size(); i++) {
    materialBuffers[i] = std::make_unique<DvmBuffer>(
        dvmDevice,
        sizeof(MaterialUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        dvmDevice.properties.limits.minUniformBufferOffsetAlignment);
    materialBuffers[i]->map();
  }
}
void SimpleRenderSystem::render(FrameInfo& frameInfo)
{
  entt::registry& registry = frameInfo.scene.getRegistry();
  dvmPipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout,
                          0,
                          1,
                          &frameInfo.globalDescriptorSet,
                          0,
                          nullptr);

  auto view = registry.view<ModelComponent, TransformComponent>();

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

void SimpleRenderSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
  frameInfo.globalDescriptorSet = globalDescriptorSets[frameInfo.frameIndex];
  uboBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
  uboBuffers[frameInfo.frameIndex]->flush();
}
}  // namespace dvm
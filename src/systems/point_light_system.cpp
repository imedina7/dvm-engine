#include "point_light_system.hpp"
#include "dvm_swap_chain.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <map>
#include <iostream>

namespace dvm
{

struct PointLightPushConstants
{
  glm::vec4 position {};
  glm::vec4 color {};
  float radius;
};

PointLightSystem::PointLightSystem(DvmDevice& device,
                                   DvmRenderer& renderer,
                                   VkDescriptorSetLayout globalSetLayout)
    : dvmDevice {device}
    , dvmRenderer {renderer}
{
  VkRenderPass renderPass = dvmRenderer.getSwapChainRenderPass();
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}
PointLightSystem::~PointLightSystem()
{
  vkDestroyPipelineLayout(dvmDevice.device(), pipelineLayout, nullptr);
}

void PointLightSystem::createPipelineLayout(
    VkDescriptorSetLayout globalSetLayout)
{
  VkPushConstantRange pushConstantRange {
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      0,
      sizeof(PointLightPushConstants)};

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

void PointLightSystem::createPipeline(VkRenderPass renderPass)
{
  PipelineConfigInfo pipelineConfig {};
  DvmPipeline::defaultPipelineConfigInfo(pipelineConfig);
  DvmPipeline::enableAlphaBlending(pipelineConfig);
  pipelineConfig.bindingDescriptions.clear();
  pipelineConfig.attributeDescriptions.clear();
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  dvmPipeline = std::make_unique<DvmPipeline>(dvmDevice,
                                              "shaders/point_light.vert.spv",
                                              "shaders/point_light.frag.spv",
                                              pipelineConfig);
}

void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
{
  entt::registry& registry = frameInfo.scene.getRegistry();

  auto rotateLight =
      glm::rotate(glm::mat4(1.f), frameInfo.frameTime, {0.f, -1.f, 0.f});
  int lightIndex = 0;
  for (auto &&[entity, pointLight, transform] : registry.view<PointLightComponent, TransformComponent>().each()) {

    assert(lightIndex < MAX_LIGHTS && "Max point lights exceeded");

    transform.translation = glm::vec3(rotateLight * glm::vec4(transform.translation, 1.f));

    ubo.pointLights[lightIndex].position =
        glm::vec4(transform.translation, 1.f);
    ubo.pointLights[lightIndex].color =
        glm::vec4(pointLight.lightColor, pointLight.lightIntensity);
    lightIndex++;
  }
  ubo.numLights = lightIndex;
}

void PointLightSystem::render(FrameInfo& frameInfo)
{
  std::map<float, entt::entity> sorted;

  entt::registry& registry = frameInfo.scene.getRegistry();
  DvmCamera& camera = frameInfo.scene.getCamera();

  for (auto&& [entity, pointLight, transform] :
       registry.view<PointLightComponent, TransformComponent>()
           .each())
  {

    glm::vec3 offset =
        camera.getPosition() - transform.translation;

    float disSquared = glm::dot(offset, offset);

    sorted[disSquared] = entity;
  }

  dvmPipeline->bind(frameInfo.commandBuffer);

  vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipelineLayout,
                          0,
                          1,
                          &frameInfo.globalDescriptorSet,
                          0,
                          nullptr);

  for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
    auto [pointLight, transform] = registry.get<PointLightComponent, TransformComponent>(it->second);

    PointLightPushConstants push {};
    push.position = glm::vec4(transform.translation, 1.f);
    push.color = glm::vec4(pointLight.lightColor, pointLight.lightIntensity);
    push.radius = transform.scale.x;

    vkCmdPushConstants(
        frameInfo.commandBuffer,
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(PointLightPushConstants),
        &push);
    vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
  }
}
}  // namespace dvm
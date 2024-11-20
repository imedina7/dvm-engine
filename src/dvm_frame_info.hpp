#pragma once

#include "dvm_camera.hpp"
#include "dvm_components.hpp"
#include <vulkan/vulkan.h>
#include <entt.hpp>

namespace dvm
{
#define MAX_LIGHTS 10

struct PointLight
{
  glm::vec4 position {};
  glm::vec4 color {};
};

struct GlobalUbo
{
  glm::mat4 projection {1.0f};
  glm::mat4 view {1.0f};
  glm::mat4 inverseView {1.0f};
  glm::vec4 ambientLightColor {1.0f, 1.0f, 1.0f, .02f};  // w is intensity
  PointLight pointLights[MAX_LIGHTS];
  int numLights;
};
struct FrameInfo
{
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  DvmCamera& camera;
  VkDescriptorSet globalDescriptorSet;
  entt::registry& registry;
};
}  // namespace dvm
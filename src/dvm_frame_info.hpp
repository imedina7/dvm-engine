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
  glm::vec4 ambientLightColor {.56f, 1.0f, 0.67f, .08f};  // w is intensity
  PointLight pointLights[MAX_LIGHTS];
  int numLights;
};

#define MAX_PHYSICS_OBJECTS 100
static constexpr const float WORLD_GRAVITY = -9.8f;

struct RigidBody
{
  glm::vec4 direction {0.f};
  glm::vec4 velocity {0.f};
  float weight;
  float friction;
  float spring;
};

struct PhysicsUbo
{
  float gravity {WORLD_GRAVITY};
  RigidBody objects[MAX_PHYSICS_OBJECTS];
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
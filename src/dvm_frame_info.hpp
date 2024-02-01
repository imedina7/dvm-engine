#pragma once

#include "dvm_camera.hpp"
#include <vulkan/vulkan.h>

namespace dvm
{
struct FrameInfo
{
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  DvmCamera& camera;
  VkDescriptorSet globalDescriptorSet;
};
}  // namespace dvm
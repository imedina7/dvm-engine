#pragma once
#include "dvm_device.hpp"
#include "dvm_swap_chain.hpp"
#include "dvm_window.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>
#include <cassert>

namespace dvm
{

// 0 Color on and Ambient off
// 1 Color on and Ambient on
// 2 Highlight on
// 3 Reflection on and Ray trace on
// 4 Transparency: Glass on
//
// Reflection: Ray trace on
// 5 Reflection: Fresnel on and Ray trace on
// 6 Transparency: Refraction on
//
// Reflection: Fresnel off and Ray trace on
// 7 Transparency: Refraction on
//
// Reflection: Fresnel on and Ray trace on
// 8 Reflection on and Ray trace off
// 9 Transparency: Glass on
//
// Reflection: Ray trace off
// 10 Casts shadows onto invisible surfaces

// enum IlluminationModel {
//   COLOR_ON_AMBIENT_OFF = 0,
//   COLOR_ON_AMBIENT_ON,
//   HIGHLIGHT_ON,
//   REFLECTION_ON_RAYTRACE_ON,
//   TRANSPARENCY_FRESNEL_OFF_RAYTRACE_ON_GLASS_ON,
//   REFLECTION_FRESNEL_ON_RAYTRACE_ON,
//   TRANSPARENCY_FRESNEL_ON_RAYTRACE_ON_REFRACTION_ON,
//   TRANSPARENCY_FRESNEL_OFF_RAYTRACE_ON_REFRACTION_ON,
//   REFLECTION_ON_FRESNEL_ON_RAYTRACE_OFF,
//   TRANSPARENCY_FRESNEL_ON_RAYTRACE_ON_GLASS_ON,
//   CAST_SHADOW_ONTO_INVISIBLE_SURFACES_ON
// }

class DvmRenderer
{
public:
  DvmRenderer(DvmWindow& window, DvmDevice& device);
  ~DvmRenderer();

  DvmRenderer(const DvmRenderer&) = delete;
  DvmRenderer& operator=(const DvmRenderer&) = delete;

  VkRenderPass getSwapChainRenderPass() const
  {
    return dvmSwapChain->getRenderPass();
  }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const
  {
    assert(isFrameStarted
           && "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getCurrentFrameIndex() const
  {
    assert(isFrameStarted
           && "Cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  float getAspectRatio() const { return dvmSwapChain->extentAspectRatio(); }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  DvmWindow& dvmWindow;
  DvmDevice& dvmDevice;
  std::unique_ptr<DvmSwapChain> dvmSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex {0};
  int currentFrameIndex {0};
  bool isFrameStarted {false};
};
}  // namespace dvm
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
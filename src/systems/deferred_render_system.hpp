#pragma once
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <vulkan/vulkan_core.h>

#include "dvm_descriptors.hpp"
#include "dvm_device.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_pipeline.hpp"
#include "dvm_renderer.hpp"
#include "dvm_swap_chain.hpp"
#include "dvm_texture.hpp"
#include "dvm_window.hpp"

namespace dvm
{
class DeferredRenderSystem
{
public:
  DeferredRenderSystem(DvmDevice& dvmDevice,
                     DvmWindow& dvmWindow);
  ~DeferredRenderSystem();

  DeferredRenderSystem(const DeferredRenderSystem&) = delete;
  DeferredRenderSystem& operator=(const DeferredRenderSystem&) = delete;

  void render(FrameInfo& frameInfo);
  void renderShadowMaps(FrameInfo& frameInfo, GlobalUbo& ubo);

  void update(FrameInfo& frameInfo, GlobalUbo& ubo);

  DvmRenderer& getRenderer() { return *dvmRenderer; };
  const VkDescriptorSetLayout getGlobalSetLayout() const { return globalSetLayout->getDescriptorSetLayout(); };
  const DvmDescriptorPool& getGlobalDescriptorPool() const { return *globalPool; }

private:
  void createDescriptorSetLayouts();
  void createPipelineLayout(
      std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
  void createPipeline(VkRenderPass renderPass);
  void createDescriptorPool();
  void createDescriptorSets();
  void createBuffers();

  void beginShadowPass(VkCommandBuffer commandBuffer);
  void endShadowPass(VkCommandBuffer commandBuffer);

  DvmDevice& dvmDevice;
  DvmWindow& dvmWindow;

  std::unique_ptr<DvmPipeline> dvmPipeline;
  VkPipelineLayout pipelineLayout;
  std::unique_ptr<DvmRenderer> dvmRenderer;
  std::unique_ptr<DvmDescriptorSetLayout> globalSetLayout {};
  std::unique_ptr<DvmDescriptorPool> globalPool {};
  std::array<VkDescriptorSet, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      globalDescriptorSets {};
  std::array<std::unique_ptr<DvmBuffer>, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      uboBuffers {};
  std::array<std::unique_ptr<DvmBuffer>, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      materialBuffers {};
  Texture texture { dvmDevice, "../assets/textures/diffuse_bake.png" };
};
}  // namespace dvm
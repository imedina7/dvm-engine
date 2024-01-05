#pragma once
#include "dvm_device.hpp"
#include "dvm_game_object.hpp"
#include "dvm_pipeline.hpp"
#include "dvm_swap_chain.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

namespace dvm {
class SimpleRenderSystem {
public:
  SimpleRenderSystem(DvmDevice& dvmDevice, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<DvmGameObject> &gameObjects);
private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  DvmDevice& dvmDevice;

  std::unique_ptr<DvmPipeline> dvmPipeline;
  VkPipelineLayout pipelineLayout;
};
} // namespace dvm
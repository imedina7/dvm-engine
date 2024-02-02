#pragma once
#include "dvm_device.hpp"
#include "dvm_game_object.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_camera.hpp"
#include "dvm_pipeline.hpp"
#include "dvm_swap_chain.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

namespace dvm
{
class PointLightSystem
{
public:
  PointLightSystem(DvmDevice& dvmDevice,
                   VkRenderPass renderPass,
                   VkDescriptorSetLayout globalSetLayout);
  ~PointLightSystem();

  PointLightSystem(const PointLightSystem&) = delete;
  PointLightSystem& operator=(const PointLightSystem&) = delete;

  void update(FrameInfo& frameInfo, GlobalUbo& ubo);
  void render(FrameInfo& frameInfo);

private:
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createPipeline(VkRenderPass renderPass);

  DvmDevice& dvmDevice;

  std::unique_ptr<DvmPipeline> dvmPipeline;
  VkPipelineLayout pipelineLayout;
};
}  // namespace dvm
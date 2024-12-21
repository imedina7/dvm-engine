#pragma once

#include "dvm_descriptors.hpp"
#include "dvm_device.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_pipeline.hpp"
#include <vulkan/vulkan_core.h>

namespace dvm
{
class PhysicsSystem
{
public:
  PhysicsSystem();
  ~PhysicsSystem();

  PhysicsSystem(const PhysicsSystem&) = delete;
  PhysicsSystem& operator=(const PhysicsSystem&) = delete;

  void update(FrameInfo& frameInfo, GlobalUbo& ubo);

private:
  void createPipeline();
  void createPipelineLayout(
      std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
  VkPipelineLayout pipelineLayout;
  std::unique_ptr<DvmPipeline> dvmPipeline;
  PhysicsUbo physicsUbo;
  DvmDevice& dvmDevice;
};
}  // namespace dvm
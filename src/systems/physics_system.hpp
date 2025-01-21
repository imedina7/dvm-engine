#pragma once

#include "dvm_descriptors.hpp"
#include "dvm_device.hpp"
#include "dvm_buffer.hpp"
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

  void update(FrameInfo& frameInfo);

private:
  void createPipeline();
  void createPipelineLayout();
  void createSyncObjects();
  void createDescriptorPool();
  void createDescriptorSets();
  void createCommandBuffer();
  void createStorageBuffers();
  void createUniformBuffers();
  void freeCommandBuffer();

private:
  std::unique_ptr<DvmDescriptorSetLayout> setLayout;
  std::unique_ptr<DvmDescriptorPool> descriptorPool;
  VkPipelineLayout pipelineLayout;
  std::unique_ptr<DvmPipeline> dvmPipeline;
  PhysicsUbo physicsUbo;
  DvmDevice& dvmDevice;
  VkCommandBuffer commandBuffer;
  std::array<std::unique_ptr<DvmBuffer>, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      uniformBuffers;
  std::array<std::unique_ptr<DvmBuffer>, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      storageBuffers;
  std::array<VkDescriptorSet, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      descriptorSets {};
  std::array<VkSemaphore, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      computeFinishedSemaphores;
  std::array<VkFence, DvmSwapChain::MAX_FRAMES_IN_FLIGHT> computeInFlightFences;
};
}  // namespace dvm
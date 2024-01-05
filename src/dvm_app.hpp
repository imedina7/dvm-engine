#pragma once
#include "dvm_device.hpp"
#include "dvm_pipeline.hpp"
#include "dvm_swap_chain.hpp"
#include "dvm_window.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace dvm {
class DvmApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  DvmApp();
  ~DvmApp();

  DvmApp(const DvmApp &) = delete;
  DvmApp &operator=(const DvmApp &) = delete;

  void run();

private:
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();

  DvmWindow dvmWindow{WIDTH, HEIGHT, "Hello vulkan!"};
  DvmDevice dvmDevice{dvmWindow};
  DvmSwapChain dvmSwapChain{dvmDevice, dvmWindow.getExtent()};
  std::unique_ptr<DvmPipeline> dvmPipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
};
} // namespace dvm
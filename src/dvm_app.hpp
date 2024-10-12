#pragma once
#include "dvm_device.hpp"
#ifdef AUDIO
#include "dvm_audio.hpp"
#endif
#include "dvm_gui.hpp"
#include "dvm_game_object.hpp"
#include "dvm_renderer.hpp"
#include "dvm_descriptors.hpp"
#include "dvm_swap_chain.hpp"
#include "dvm_window.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

namespace dvm
{
class DvmApp
{
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  DvmDevice& getDevice() { return dvmDevice; }
  DvmWindow& getWindow() { return dvmWindow; }
  DvmRenderer& getRenderer() { return dvmRenderer; }

  const DvmDescriptorPool& getGlobalDescriptorPool() const
  {
    return *globalPool;
  }

  static DvmApp& getInstance()
  {
    static DvmApp m_instance;
    return m_instance;
  }

  DvmApp(const DvmApp&) = delete;
  DvmApp& operator=(const DvmApp&) = delete;

  void run();

private:
  DvmApp()
  {
    globalPool = DvmDescriptorPool::Builder(dvmDevice)
                     .setMaxSets(DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                  DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .build();
    loadGameObjects();
  };
  ~DvmApp() {}
  void loadGameObjects();

  DvmWindow dvmWindow {WIDTH, HEIGHT, "Hello vulkan!"};
  DvmDevice dvmDevice {dvmWindow};
  DvmRenderer dvmRenderer {dvmWindow, dvmDevice};
  std::unique_ptr<DvmDescriptorPool> globalPool {};
  DvmGameObject::Map gameObjects;
};
}  // namespace dvm
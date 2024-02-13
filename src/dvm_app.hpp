#pragma once
#include "dvm_device.hpp"
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
#include <thread>

namespace dvm
{
class DvmApp
{
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  DvmApp();
  ~DvmApp();

  DvmApp(const DvmApp&) = delete;
  DvmApp& operator=(const DvmApp&) = delete;

  void run();

private:
  void loadGameObjects();

  DvmWindow dvmWindow {WIDTH, HEIGHT, "Hello vulkan!"};
  DvmDevice dvmDevice {dvmWindow};
  DvmRenderer dvmRenderer {dvmWindow, dvmDevice};
  std::thread audio;

  std::unique_ptr<DvmDescriptorPool> globalPool {};
  DvmGameObject::Map gameObjects;
};
}  // namespace dvm
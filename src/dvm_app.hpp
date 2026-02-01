#pragma once

#include "dvm_device.hpp"

#ifdef AUDIO
#  include "dvm_audio.hpp"
#endif

#include "dvm_scene.hpp"
#include "dvm_window.hpp"
#include "core/layer_stack.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

#ifndef WINDOW_WIDTH
#  define WINDOW_WIDTH 1280
#endif

#ifndef WINDOW_HEIGHT
#  define WINDOW_HEIGHT 720
#endif

#ifndef WINDOW_TITLE
#  define WINDOW_TITLE "DVMtech - DVM Engine"
#endif

namespace dvm
{
class DvmApp
{
public:
  static constexpr int WIDTH = WINDOW_WIDTH;
  static constexpr int HEIGHT = WINDOW_HEIGHT;

  DvmDevice& getDevice() { return m_dvmDevice; }
  DvmWindow& getWindow() { return m_dvmWindow; }

  static DvmApp& getInstance()
  {
    static DvmApp s_instance;
    return s_instance;
  }

  DvmApp(const DvmApp&) = delete;
  auto operator=(const DvmApp&) -> DvmApp& = delete;

  void run();

  Scene& getScene() { return m_scene; };

private:
  DvmApp() = default;
  ~DvmApp() {}

  DvmWindow m_dvmWindow {WIDTH, HEIGHT, WINDOW_TITLE};
  DvmDevice m_dvmDevice {m_dvmWindow};
  LayerStack m_layerStack;
  Scene m_scene;
};
}  // namespace dvm
#pragma once

#include "dvm_device.hpp"

#ifdef AUDIO
#  include "dvm_audio.hpp"
#endif

#include "dvm_gui.hpp"
#include "dvm_scene.hpp"
#include "dvm_window.hpp"

#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/constants.hpp>

#ifndef WINDOW_WIDTH
#  define WINDOW_WIDTH 800
#endif

#ifndef WINDOW_HEIGHT
#  define WINDOW_HEIGHT 600
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

  DvmDevice& getDevice() { return dvmDevice; }
  DvmWindow& getWindow() { return dvmWindow; }

  static DvmApp& getInstance()
  {
    static DvmApp m_instance;
    return m_instance;
  }

  DvmApp(const DvmApp&) = delete;
  DvmApp& operator=(const DvmApp&) = delete;

  void run();

  Scene& getScene() { return m_Scene; };

private:
  DvmApp() = default;
  ~DvmApp() {}

  DvmWindow dvmWindow {WIDTH, HEIGHT, WINDOW_TITLE};
  DvmDevice dvmDevice {dvmWindow};
  Scene m_Scene {};
};
}  // namespace dvm
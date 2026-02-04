#pragma once

#include "dvm_device.hpp"
#include "dvm_window.hpp"
#include "core/layer_stack.hpp"
#include "core/application_context.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
// Forward declarations
class Event;
class Layer;

class DvmApp
{
public:
  static constexpr int WIDTH = WINDOW_WIDTH;
  static constexpr int HEIGHT = WINDOW_HEIGHT;

  static DvmApp& getInstance()
  {
    static DvmApp s_instance;
    return s_instance;
  }

  DvmApp(const DvmApp&) = delete;
  auto operator=(const DvmApp&) -> DvmApp& = delete;

  void run();

  // Layer management
  void pushLayer(Layer* layer);
  void pushOverlay(Layer* overlay);

  // Accessors
  DvmDevice& getDevice() { return m_dvmDevice; }
  DvmWindow& getWindow() { return m_dvmWindow; }
  ApplicationContext& getContext() { return m_context; }

private:
  DvmApp();
  ~DvmApp();

  void initializeLayers();
  void setupEventCallbacks();
  void onEvent(Event& e);

  DvmWindow m_dvmWindow {WIDTH, HEIGHT, WINDOW_TITLE};
  DvmDevice m_dvmDevice {m_dvmWindow};
  ApplicationContext m_context;
  LayerStack m_layerStack;

  // For mouse tracking
  glm::vec2 m_lastMousePos {0.0f};
};
}  // namespace dvm
#include "dvm_app.hpp"
#include "core/events/application_event.hpp"
#include "core/events/key_event.hpp"
#include "core/events/mouse_event.hpp"
#include "layers/scene_layer.hpp"
#include "layers/render_layer.hpp"
#include "layers/imgui_layer.hpp"
#include <chrono>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#ifdef AUDIO
#  include "dvm_audio.hpp"
#endif

namespace dvm
{
DvmApp::DvmApp()
{
  // Setup context
  m_context.device = &m_dvmDevice;
  m_context.window = &m_dvmWindow;

  setupEventCallbacks();
  initializeLayers();
}

DvmApp::~DvmApp()
{
  vkDeviceWaitIdle(m_dvmDevice.device());
}

void DvmApp::initializeLayers()
{
  // Create layers (order matters!)
  auto* sceneLayer = new SceneLayer(m_context);
  auto* renderLayer = new RenderLayer(m_context);

  m_layerStack.pushLayer(sceneLayer);
  m_layerStack.pushLayer(renderLayer);

  // Attach scene and render layers first
  sceneLayer->onAttach();
  renderLayer->onAttach();

  // // ImGui as overlay (renders on top)
  // // IMPORTANT: Create after renderLayer->onAttach() so renderer is initialized
  // auto* imguiLayer = new ImGuiLayer(m_context, renderLayer->getRenderer());
  // m_layerStack.pushOverlay(imguiLayer);
  // imguiLayer->onAttach();
}

void DvmApp::setupEventCallbacks()
{
  GLFWwindow* window = m_dvmWindow.getGLFWwindow();
  glfwSetWindowUserPointer(window, this);

  // Mouse position callback
  glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
    auto* app = static_cast<DvmApp*>(glfwGetWindowUserPointer(w));
    glm::vec2 current(x, y);
    glm::vec2 delta = current - app->m_lastMousePos;
    app->m_lastMousePos = current;

    // Update context
    app->m_context.mousePosition = current;
    app->m_context.mouseDelta = delta;

    // Dispatch event
    MouseMovedEvent event(x, y, delta.x, delta.y);
    app->onEvent(event);
  });

  // Key callback
  glfwSetKeyCallback(
      window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto* app = static_cast<DvmApp*>(glfwGetWindowUserPointer(w));

        // Escape to close
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
          glfwSetWindowShouldClose(w, GLFW_TRUE);
          return;
        }

#ifdef AUDIO
        // Audio test
        if (key == GLFW_KEY_F && action == GLFW_PRESS) {
          DvmAudio& audio = DvmAudio::Get();
          audio.playFromFile(AUDIO_FILE_PATH);
        }
#endif

        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
          KeyPressedEvent event(key, action == GLFW_REPEAT);
          app->onEvent(event);
        } else if (action == GLFW_RELEASE) {
          KeyReleasedEvent event(key);
          app->onEvent(event);
        }
      });

  // Mouse button callback
  glfwSetMouseButtonCallback(
      window, [](GLFWwindow* w, int button, int action, int mods) {
        auto* app = static_cast<DvmApp*>(glfwGetWindowUserPointer(w));

        if (action == GLFW_PRESS) {
          MouseButtonPressedEvent event(button);
          app->onEvent(event);
        } else if (action == GLFW_RELEASE) {
          MouseButtonReleasedEvent event(button);
          app->onEvent(event);
        }
      });

  // Window resize callback
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow* w, int width, int height) {
        auto* app = static_cast<DvmApp*>(glfwGetWindowUserPointer(w));
        WindowResizeEvent event(width, height);
        app->onEvent(event);
      });

  // Setup mouse mode
  double mouseX, mouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);
  m_lastMousePos = glm::vec2(mouseX, mouseY);

  if (glfwRawMouseMotionSupported() == GLFW_TRUE) {
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void DvmApp::onEvent(Event& e)
{
  // Propagate to layers in reverse order (overlays first)
  for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it) {
    if (e.isHandled)
      break;
    (*it)->onEvent(e);
  }
}

void DvmApp::run()
{
  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!m_dvmWindow.shouldClose()) {
    glfwPollEvents();  // Triggers callbacks

    // Calculate delta time
    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(
            newTime - currentTime)
            .count();
    currentTime = newTime;

    m_context.deltaTime = frameTime;

    // Dispatch tick event
    AppTickEvent tickEvent(frameTime);
    onEvent(tickEvent);

    // Update all layers
    for (auto* layer : m_layerStack) {
      layer->onUpdate(frameTime);
    }

    // Render layers (if they implement onRender)
    for (auto* layer : m_layerStack) {
      layer->onRender();
    }

    // UI render pass (overlays)
    for (auto* layer : m_layerStack) {
      layer->onUIRender();
    }
  }
}

void DvmApp::pushLayer(Layer* layer)
{
  m_layerStack.pushLayer(layer);
  layer->onAttach();
}

void DvmApp::pushOverlay(Layer* overlay)
{
  m_layerStack.pushOverlay(overlay);
  overlay->onAttach();
}

}  // namespace dvm

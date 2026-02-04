#include "imgui_layer.hpp"
#include "../core/events/key_event.hpp"
#include "../core/events/mouse_event.hpp"
#include "../dvm_frame_info.hpp"
#include <GLFW/glfw3.h>

namespace dvm
{
void ImGuiLayer::onAttach()
{
  m_gui = std::make_unique<DvmGUI>(m_renderer);
}

void ImGuiLayer::onUpdate(float dt)
{
  m_gui->checkUIToggle();
  m_uiVisible = m_gui->getUIVisibility();

  // Update context so other layers know if camera control is allowed
  m_context.cameraControlEnabled = !m_uiVisible;
}

void ImGuiLayer::onUIRender()
{
  if (!m_uiVisible || !m_context.activeScene) {
    return;
  }

  FrameInfo frameInfo {m_renderer.getCurrentFrameIndex(),
                       m_context.deltaTime,
                       m_renderer.getCurrentCommandBuffer(),
                       *m_context.activeScene};

  m_gui->render(frameInfo);
}

void ImGuiLayer::onEvent(Event& e)
{
  EventDispatcher dispatcher(e);

  // UI toggle with F1 key
  dispatcher.dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) {
    if (event.getKeyCode() == GLFW_KEY_F1) {
      m_gui->toggleUI();
      m_uiVisible = m_gui->getUIVisibility();
      m_context.cameraControlEnabled = !m_uiVisible;
      return true;  // Consume toggle event
    }
    return false;
  });

  // If UI is visible, consume input events
  if (m_uiVisible) {
    dispatcher.dispatch<MouseMovedEvent>([](MouseMovedEvent& event) {
      return true;  // Consume all mouse when UI visible
    });

    dispatcher.dispatch<MouseButtonPressedEvent>(
        [](MouseButtonPressedEvent& event) {
          return true;  // Consume mouse buttons
        });

    dispatcher.dispatch<MouseButtonReleasedEvent>(
        [](MouseButtonReleasedEvent& event) {
          return true;  // Consume mouse buttons
        });

    dispatcher.dispatch<KeyPressedEvent>([](KeyPressedEvent& event) {
      return true;  // Consume all keys when UI visible
    });
  }
}

void ImGuiLayer::onDetach()
{
  m_gui.reset();
}
}  // namespace dvm

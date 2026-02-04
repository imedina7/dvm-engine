#include "scene_layer.hpp"
#include "../dvm_frame_info.hpp"

namespace dvm
{
void SceneLayer::onAttach()
{
  m_scene.load();
  m_context.activeScene = &m_scene;
}

void SceneLayer::onUpdate(float dt)
{
  // Update scene with current input state from context
  m_context.currentFrameUbo = m_scene.update(dt,
                                              m_context.mouseDelta,
                                              m_context.cameraControlEnabled,
                                              m_context.aspectRatio);

  // Reset mouse delta after use (consumed by scene update)
  m_context.mouseDelta = glm::vec2(0.0f);
}

void SceneLayer::onEvent(Event& e)
{
  EventDispatcher dispatcher(e);

  // Handle scene-specific events if needed
  dispatcher.dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) {
    // Camera movement is handled by scene update with input state
    // Could add scene-specific key handling here
    return false;  // Don't consume
  });
}

void SceneLayer::onDetach()
{
  m_context.activeScene = nullptr;
}
}  // namespace dvm

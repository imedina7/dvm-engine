#pragma once

#include "../core/layer.hpp"
#include "../core/application_context.hpp"
#include "../core/events/key_event.hpp"
#include "../core/events/mouse_event.hpp"
#include "../dvm_scene.hpp"

namespace dvm
{
class SceneLayer : public Layer
{
public:
  SceneLayer(ApplicationContext& context)
      : Layer("SceneLayer", context)
  {
  }

  void onAttach() override;
  void onUpdate(float dt) override;
  void onEvent(Event& e) override;
  void onDetach() override;

  Scene& getScene() { return m_scene; }

private:
  Scene m_scene;
};
}  // namespace dvm

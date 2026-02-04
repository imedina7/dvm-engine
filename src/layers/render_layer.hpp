#pragma once

#include "../core/layer.hpp"
#include "../core/application_context.hpp"
#include "../core/events/application_event.hpp"
#include "../systems/deferred_render_system.hpp"
#include "../systems/point_light_system.hpp"
#include <memory>

namespace dvm
{
class RenderLayer : public Layer
{
public:
  RenderLayer(ApplicationContext& context)
      : Layer("RenderLayer", context)
  {
  }

  void onAttach() override;
  void onUpdate(float dt) override;
  void onRender() override;
  void onEvent(Event& e) override;
  void onDetach() override;

  DvmRenderer& getRenderer() { return *m_renderer; }

private:
  std::unique_ptr<DeferredRenderSystem> m_deferredRenderSystem;
  std::unique_ptr<PointLightSystem> m_pointLightSystem;
  DvmRenderer* m_renderer = nullptr;  // Reference from deferred system
};
}  // namespace dvm

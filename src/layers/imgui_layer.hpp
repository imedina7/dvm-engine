#pragma once

#include "../core/layer.hpp"
#include "../core/application_context.hpp"
#include "../dvm_gui.hpp"
#include "../dvm_renderer.hpp"
#include <memory>

namespace dvm
{
class ImGuiLayer : public Layer
{
public:
  ImGuiLayer(ApplicationContext& context, DvmRenderer& renderer)
      : Layer("ImGuiLayer", context), m_renderer(renderer)
  {
  }

  void onAttach() override;
  void onUpdate(float dt) override;
  void onUIRender() override;
  void onEvent(Event& e) override;
  void onDetach() override;

private:
  DvmRenderer& m_renderer;
  std::unique_ptr<DvmGUI> m_gui;
  bool m_uiVisible = false;
};
}  // namespace dvm

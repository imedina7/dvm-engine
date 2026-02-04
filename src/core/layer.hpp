#pragma once

#include "event.hpp"
#include "application_context.hpp"

namespace dvm
{
class Layer
{
public:
  Layer(const std::string& _layerName, ApplicationContext& context);
  virtual ~Layer() = default;

  virtual void onAttach() {};
  virtual void onEvent(Event& e) {};
  virtual void onUpdate(float dt) {};
  virtual void onRender() {};
  virtual void onUIRender() {};
  virtual void onDetach() {};

  const std::string& getName() const { return layerName; }

protected:
  std::string layerName;
  ApplicationContext& m_context;
};
}  // namespace dvm
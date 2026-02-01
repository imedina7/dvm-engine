#include "event.hpp"

namespace dvm
{
class Layer
{
public:
  Layer(const std::string& _layerName = "Layer");
  virtual ~Layer() = default;

  virtual void onAttach() {};
  virtual void onEvent(Event& e) {};
  virtual void onUpdate(float dt) {};
  virtual void onUIRender() {};
  virtual void onDetach() {};

  const std::string& getName() const { return layerName; }

protected:
  std::string layerName;
};
}  // namespace dvm
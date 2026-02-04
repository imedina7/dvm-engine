#include "layer.hpp"

namespace dvm
{
Layer::Layer(const std::string& _layerName, ApplicationContext& context)
    : layerName {_layerName}, m_context {context}
{
}
}  // namespace dvm
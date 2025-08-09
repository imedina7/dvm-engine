#include "dvm_entity.hpp"

namespace dvm {
      Entity::Entity(entt::entity entity, Scene* scene)
      : m_Id{entity}, m_Scene{scene}
  {}
}
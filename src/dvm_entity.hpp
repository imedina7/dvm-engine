#pragma once
#include <string>
#include <cassert>
#include <entt.hpp>
#include "dvm_scene.hpp"
namespace dvm
{
class Entity
{
public:
  Entity() = default;
  Entity(entt::entity entity, Scene* scene);
  Entity(const Entity& other) = default;

  entt::entity getId() const { return m_Id; }

  template<typename Component>
  bool hasComponent()
  {
    return m_Scene->registry.template all_of<Component>(m_Id);
  }

  template<typename Component>
  Component& getComponent() {
    assert(hasComponent<Component>() && "Component does not exist in entity");

    return m_Scene->registry.template get<Component>(m_Id);
  }

  template<typename Component, typename... Args>
  Component& addComponent(Args&&... args) {
    assert(!hasComponent<Component>() && "Component already exists in entity");
    return m_Scene->registry.template emplace<Component>(m_Id, std::forward<Args>(args)...);
  }

  template<typename Component, typename... Args>
  Component& updateComponent(Args&&... args) {
    assert(hasComponent<Component>() && "Component does not exist in entity");
    return m_Scene->registry.template replace<Component>(m_Id, std::forward<Args>(args)...);
  }
  template<typename Component, typename... Args>
  Component& patchComponent(Args&&... args) {
    assert(hasComponent<Component>() && "Component does not exist in entity");
    return m_Scene->registry.template patch<Component>(m_Id, std::forward<Args>(args)...);
  }

private:
  entt::entity m_Id { entt::null };
  Scene *m_Scene = nullptr;
};
}  // namespace dvm

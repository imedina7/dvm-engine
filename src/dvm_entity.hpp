#pragma once
#include <string>
#include <cassert>

namespace dvm
{
class Scene;
class Entity
{
public:
  Entity() = default;
  Entity(entt::entity entity, Scene* scene)
      : m_Id{entity}, m_Scene{scene}
  {}
  Entity(const Entity& other) = default;

  entt::entity getId() const { return m_Id; }

  template<typename Component>
  bool hasComponent() const
  {
    return m_Scene->registry.all_of<Component>(m_Id);
  }

  template<typename Component>
  Component& getComponent() {
    assert(hasComponent<Component>() && "Component does not exist in entity");

    return m_Scene->registry.get<Component>(m_Id);
  }

  template<typename Component, typename... Args>
  Component& addComponent(Args&&... args) {
    assert(!hasComponent<Component>() && "Component already exists in entity");
    return m_Scene->registry.emplace<Component>(m_Id, std::forward<Args>(args)...);
  }

  template<typename Component, typename... Args>
  Component& updateComponent(Args&&... args) {
    assert(hasComponent<Component>() && "Component does not exist in entity");
    return m_Scene->registry.replace<Component>(m_Id, std::forward<Args>(args)...);
  }
  template<typename Component, typename... Args>
  Component& patchComponent(Args&&... args) {
    assert(hasComponent<Component>() && "Component does not exist in entity");
    return m_Scene->registry.patch<Component>(m_Id, std::forward<Args>(args)...);
  }

private:
  entt::entity m_Id { entt::null };
  Scene *m_Scene = nullptr;
};
}  // namespace dvm

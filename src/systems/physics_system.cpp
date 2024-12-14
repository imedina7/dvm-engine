#include "physics_system.hpp"
namespace dvm
{
void PhysicsSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
{
  auto view = frameInfo.registry.view<PhysicsMaterialComponent,
                                      RigidBodyComponent,
                                      TransformComponent,
                                      CollisionObject>();

  for (auto&& [entity, material, rigidBody, transform, collision] : view.each())
  {
  }
}
}  // namespace dvm
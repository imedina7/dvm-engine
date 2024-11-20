#pragma once

#include <entt.hpp>
#include "dvm_components.hpp"
#include "dvm_camera.hpp"

namespace dvm
{
class FPSMovementController
{
public:
  FPSMovementController() = default;
  FPSMovementController(GLFWwindow* _window)
      : window {_window}
  {}
  struct KeyMappings
  {
    int moveForward = GLFW_KEY_W;
    int moveBackward = GLFW_KEY_S;
    int moveLeft = GLFW_KEY_A;
    int moveRight = GLFW_KEY_D;
    int moveUp = GLFW_KEY_E;
    int moveDown = GLFW_KEY_Q;

    int lookUp = GLFW_KEY_UP;
    int lookDown = GLFW_KEY_DOWN;
    int lookLeft = GLFW_KEY_LEFT;
    int lookRight = GLFW_KEY_RIGHT;
  };

  void moveInPlaneXZ(DvmCamera& camera,
                     float dt,
                     entt::registry& registry,
                     entt::entity entity,
                     glm::vec2 deltaCursor,
                     float mouseSensitivity);

  KeyMappings keys {};

  float moveSpeed {3.f};
  float lookSpeed {1.5f};

private:
  GLFWwindow* window;
};
}  // namespace dvm
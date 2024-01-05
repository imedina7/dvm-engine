#pragma once

#include "dvm_game_object.hpp"

namespace dvm
{
class KeyboardMovementController
{
public:
  struct KeyMappings
  {
    int moveForward = GLFW_KEY_W;
    int moveBackward = GLFW_KEY_S;
    int moveLeft = GLFW_KEY_A;
    int moveRight = GLFW_KEY_D;
    int moveUp = GLFW_KEY_Q;
    int moveDown = GLFW_KEY_E;

    int lookUp = GLFW_KEY_UP;
    int lookDown = GLFW_KEY_DOWN;
    int lookLeft = GLFW_KEY_LEFT;
    int lookRight = GLFW_KEY_RIGHT;
  };

  void moveInPlaneXZ(GLFWwindow* window, float dt, DvmGameObject& gameObj);

  KeyMappings keys {};

  float moveSpeed {3.f};
  float lookSpeed {1.5f};
};
}  // namespace dvm
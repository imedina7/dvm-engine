#pragma once

#include <entt.hpp>
#include "dvm_components.hpp"
#include "dvm_camera.hpp"
#include "dvm_entity.hpp"
#include "dvm_input.hpp"

namespace dvm
{
class FPSMovementController
{
public:
  FPSMovementController() = default;
  ~FPSMovementController() = default;
  struct KeyMappings
  {
    Input::KeyCode moveForward = Input::KeyCode::KEY_W;
    Input::KeyCode moveBackward = Input::KeyCode::KEY_S;
    Input::KeyCode moveLeft = Input::KeyCode::KEY_A;
    Input::KeyCode moveRight = Input::KeyCode::KEY_D;
    Input::KeyCode moveUp = Input::KeyCode::KEY_E;
    Input::KeyCode moveDown = Input::KeyCode::KEY_Q;

    Input::KeyCode lookUp = Input::KeyCode::KEY_UP;
    Input::KeyCode lookDown = Input::KeyCode::KEY_DOWN;
    Input::KeyCode lookLeft = Input::KeyCode::KEY_LEFT;
    Input::KeyCode lookRight = Input::KeyCode::KEY_RIGHT;
  };

  struct GamepadMappings
  {
    Input::GamepadAxis moveZ = Input::GamepadAxis::LEFT_Y;
    Input::GamepadAxis moveX = Input::GamepadAxis::LEFT_X;
    Input::GamepadAxis moveUp = Input::GamepadAxis::RIGHT_TRIGGER;
    Input::GamepadAxis moveDown = Input::GamepadAxis::LEFT_TRIGGER;

    Input::GamepadAxis lookUpDown = Input::GamepadAxis::RIGHT_Y;
    Input::GamepadAxis lookSides = Input::GamepadAxis::RIGHT_X;
  };

  void moveInPlaneXZ(Entity& cameraEntity,
                     float dt,
                     glm::vec2 deltaCursor,
                     float mouseSensitivity);

  KeyMappings keyboard {};
  GamepadMappings gamepad {};

  float moveSpeed {3.f};
  float lookSpeed {1.5f};
};
}  // namespace dvm
#include "keyboard_movement_controller.hpp"
#include "dvm_app.hpp"
#include <limits>
#include <iostream>
#include <entt.hpp>

#include <glm/gtc/constants.hpp>
namespace dvm
{
void FPSMovementController::moveInPlaneXZ(Entity& cameraEntity,
                                          float dt,
                                          glm::vec2 deltaCursor,
                                          float sensitivity)
{
  glm::vec2 lookDelta {0.f};
  glm::vec2 moveDelta {0.f};
  glm::vec3 rotate {0.f};

  if (Input::isKeyPressed(keyboard.lookRight))
    lookDelta.y += 1.f;
  if (Input::isKeyPressed(keyboard.lookLeft))
    lookDelta.y -= 1.f;
  if (Input::isKeyPressed(keyboard.lookUp))
    lookDelta.x += 1.f;
  if (Input::isKeyPressed(keyboard.lookDown))
    lookDelta.x -= 1.f;

  std::vector<float> axes = Input::getGamepadAxes(Input::getDefaultJoystick());

  float gamepadElevate = 0, gamepadDescend = 0;

  if (axes.size() >= 6) {
    lookDelta.x = axes.at(static_cast<size_t>(gamepad.lookSides));
    lookDelta.y = axes.at(static_cast<size_t>(gamepad.lookUpDown));
    moveDelta.y = axes.at(static_cast<size_t>(gamepad.moveZ));
    moveDelta.x = axes.at(static_cast<size_t>(gamepad.moveX));
    gamepadElevate = axes.at(static_cast<size_t>(gamepad.moveUp));
    gamepadDescend = axes.at(static_cast<size_t>(gamepad.moveDown));
  }

  if (glm::abs(lookDelta.x) > 0.05f) {
    rotate.y += lookDelta.x;
  }
  if (glm::abs(lookDelta.y) > 0.05f) {
    rotate.x -= lookDelta.y;
  }

  auto& cameraComponent = cameraEntity.getComponent<CameraComponent>();

  cameraEntity.patchComponent<TransformComponent>(
      [&](auto& transform)
      {
        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
          transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        transform.rotation.x -= deltaCursor.y * dt * sensitivity;

        transform.rotation.y += deltaCursor.x * dt * sensitivity;

        transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
        transform.rotation.y =
            glm::mod(transform.rotation.y, glm::two_pi<float>());

        float yaw = transform.rotation.y;
        const glm::vec3 forwardDir {sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir {forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir {0.f, -1.f, 0.f};

        glm::vec3 moveDir {0.f};

        if (Input::isKeyPressed(keyboard.moveForward))
          moveDir += forwardDir;
        if (Input::isKeyPressed(keyboard.moveBackward))
          moveDir -= forwardDir;
        if (Input::isKeyPressed(keyboard.moveRight))
          moveDir += rightDir;
        if (Input::isKeyPressed(keyboard.moveLeft))
          moveDir -= rightDir;
        if (Input::isKeyPressed(keyboard.moveUp))
          moveDir += upDir;
        if (Input::isKeyPressed(keyboard.moveDown))
          moveDir -= upDir;

        if (glm::abs(moveDelta.x) > 0.05f) {
          moveDir += rightDir * moveDelta.x;
        }
        if (glm::abs(moveDelta.y) > 0.05f) {
          moveDir -= forwardDir * moveDelta.y;
        }

        if (glm::abs(gamepadElevate) > 0.02f)
          moveDir += upDir * gamepadElevate;
        if (glm::abs(gamepadDescend) > 0.02f)
          moveDir -= upDir * gamepadDescend;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
          transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
        cameraComponent.camera.setViewYXZ(transform.translation,
                                         transform.rotation);
      });
}
}  // namespace dvm
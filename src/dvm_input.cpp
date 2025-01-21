#include "dvm_app.hpp"
#include "dvm_input.hpp"

namespace dvm
{
bool Input::isKeyPressed(Input::KeyCode key)
{
  GLFWwindow* window = DvmApp::getInstance().getWindow().getGLFWwindow();
  return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}
bool Input::isKeyReleased(Input::KeyCode key)
{
  GLFWwindow* window = DvmApp::getInstance().getWindow().getGLFWwindow();
  return glfwGetKey(window, static_cast<int>(key)) == GLFW_RELEASE;
}
std::vector<float> Input::getGamepadAxes(Input::JoystickId joystick,
                                         int axisCount)
{
  GLFWwindow* window = DvmApp::getInstance().getWindow().getGLFWwindow();
  const float* axesValues =
      glfwGetJoystickAxes(static_cast<int>(joystick), &axisCount);

  if (axesValues == NULL) {
    return {};
  }

  // std::cout << "axis 0: " << axesValues[0] << "\n";
  // std::cout << "axis 1: " << axesValues[1] << "\n";
  // std::cout << "axis 2: " << axesValues[2] << "\n";
  // std::cout << "axis 3: " << axesValues[3] << "\n";
  // std::cout << "axis 4: " << axesValues[4] << "\n";
  // std::cout << "axis 5: " << axesValues[5] << "\n";

  std::vector<float> axes;
  for (int i = 0; i < axisCount; i++) {
    axes.push_back(axesValues[i]);
  }
  return axes;
}

bool Input::isGamepadButtonPressed(Input::JoystickId joystick,
                                   Input::GamepadButton button)
{
  GLFWgamepadstate* gamepadState = NULL;
  if (glfwGetGamepadState(static_cast<int>(joystick), gamepadState)
      == GLFW_FALSE)
  {
    std::cout << "No gamepad connected\n";
    return false;
  }
  return gamepadState->buttons[static_cast<int>(button)] == 255;
}

}  // namespace dvm
#include "dvm_app.hpp"
#include "dvm_camera.hpp"
#include "dvm_swap_chain.hpp"
#include "keyboard_movement_controller.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <vulkan/vulkan_core.h>

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace dvm
{
DvmApp::DvmApp()
{
  loadGameObjects();
}
DvmApp::~DvmApp() {}

void DvmApp::run()
{
  SimpleRenderSystem simpleRenderSystem {dvmDevice,
                                         dvmRenderer.getSwapChainRenderPass()};
  DvmCamera camera {};
  GLFWwindow* window = dvmWindow.getGLFWwindow();

  camera.setViewDirection(glm::vec3(0.f, 3.f, 2.5f), glm::vec3(0.f, 0.f, 0.f));

  auto currentTime = std::chrono::high_resolution_clock::now();

  auto viewerObject = DvmGameObject::createGameObject();
  KeyboardMovementController cameraController {};

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  double mouseInitX, mouseInitY;
  glfwGetCursorPos(window, &mouseInitX, &mouseInitY);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  while (!dvmWindow.shouldClose()
         && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
  {
    glfwPollEvents();
    double mouseNewX, mouseNewY;
    glfwGetCursorPos(window, &mouseNewX, &mouseNewY);

    glm::vec2 mouseDelta {mouseNewX - mouseInitX, mouseNewY - mouseInitY};
    mouseInitX = mouseNewX;
    mouseInitY = mouseNewY;
    auto newTime = std::chrono::high_resolution_clock::now();

    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(
            newTime - currentTime)
            .count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(
        window, frameTime, viewerObject, mouseDelta, 0.1f);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = dvmRenderer.getAspectRatio();

    camera.setPerspectiveProjection(glm::radians(50.f), aspect, .06f, 100.f);

    if (auto commandBuffer = dvmRenderer.beginFrame()) {
      dvmRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      dvmRenderer.endSwapChainRenderPass(commandBuffer);
      dvmRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(dvmDevice.device());
}

void DvmApp::loadGameObjects()
{
  std::shared_ptr<DvmModel> model = DvmModel::createModelFromFile(
      dvmDevice,
      "C:\\Users\\israel.medina\\projects\\personal\\dvm-engine\\models\\flat_"
      "vase.obj");

  auto vase = DvmGameObject::createGameObject();
  vase.model = model;
  vase.transform.translation = {
      0.f,
      .5f,
      2.f,
  };
  vase.transform.scale = {1.5f, 1.5f, 1.5f};
  gameObjects.push_back(std::move(vase));

  std::shared_ptr<DvmModel> smoothVaseModel = DvmModel::createModelFromFile(
      dvmDevice,
      "C:\\Users\\israel.medina\\projects\\personal\\dvm-"
      "engine\\models\\smooth_"
      "vase.obj");

  auto smoothVase = DvmGameObject::createGameObject();
  smoothVase.model = smoothVaseModel;
  smoothVase.transform.translation = {
      1.f,
      .5f,
      2.f,
  };
  smoothVase.transform.scale = {1.5f, 1.5f, 1.5f};
  gameObjects.push_back(std::move(smoothVase));
}
}  // namespace dvm
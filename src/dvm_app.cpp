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

  camera.setViewTarget(glm::vec3(-5.f, -2.f, -7.f), glm::vec3(0.f, 0.f, 2.5f));

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

    cameraController.moveInPlaneXZ(window, frameTime, viewerObject, mouseDelta, 0.1f);
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

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<DvmModel> createCubeModel(DvmDevice& device, glm::vec3 offset)
{
  std::vector<DvmModel::Vertex> vertices {

      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<DvmModel>(device, vertices);
}

void DvmApp::loadGameObjects()
{
  std::shared_ptr<DvmModel> model =
      createCubeModel(dvmDevice, glm::vec3 {0.f, 0.f, 0.f});

  auto cube = DvmGameObject::createGameObject();
  cube.model = model;
  cube.transform.translation = {
      0.f,
      0.f,
      2.5f,
  };
  cube.transform.scale = {.5f, .5f, .5f};
  gameObjects.push_back(std::move(cube));
}
}  // namespace dvm
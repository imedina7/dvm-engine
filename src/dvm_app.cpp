#include "dvm_app.hpp"
#include "dvm_buffer.hpp"
#include "dvm_camera.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_swap_chain.hpp"
#include "keyboard_movement_controller.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"

#include "dvm_texture.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace dvm
{
void DvmApp::run()
{
  std::array<std::unique_ptr<DvmBuffer>, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      uboBuffers;

  int time = 0;

  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<DvmBuffer>(
        dvmDevice,
        sizeof(GlobalUbo),
        DvmSwapChain::MAX_FRAMES_IN_FLIGHT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        dvmDevice.properties.limits.minUniformBufferOffsetAlignment);
    uboBuffers[i]->map();
  }

  auto globalSetLayout =
      DvmDescriptorSetLayout::Builder(dvmDevice)
          .addBinding(0,
                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                      VK_SHADER_STAGE_ALL_GRAPHICS)
          .addBinding(1,
                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_ALL_GRAPHICS)
          .build();

  Texture texture = Texture(dvmDevice, "../textures/image.png");
  VkDescriptorImageInfo imageInfo {};
  imageInfo.sampler = texture.getSampler();
  imageInfo.imageLayout = texture.getImageLayout();
  imageInfo.imageView = texture.getImageView();

  std::array<VkDescriptorSet, DvmSwapChain::MAX_FRAMES_IN_FLIGHT>
      globalDescriptorSets {};

  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    DvmDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .writeImage(1, &imageInfo)
        .build(globalDescriptorSets[i]);
  }

  SimpleRenderSystem simpleRenderSystem {
      dvmDevice,
      dvmRenderer.getSwapChainRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};

  PointLightSystem pointLightSystem {dvmDevice,
                                     dvmRenderer.getSwapChainRenderPass(),
                                     globalSetLayout->getDescriptorSetLayout()};

  DvmCamera camera {};
  GLFWwindow* window = dvmWindow.getGLFWwindow();

  camera.setViewDirection(glm::vec3(0.f, 3.f, 2.5f), glm::vec3(0.f, 0.f, 0.f));

  auto currentTime = std::chrono::high_resolution_clock::now();

  auto viewerObject = DvmGameObject::createGameObject();
  KeyboardMovementController cameraController {};
  viewerObject.transform.translation.z = -2.5f;

  double mouseInitX, mouseInitY;
  glfwGetCursorPos(window, &mouseInitX, &mouseInitY);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  DvmGUI dvmGui {};
#ifdef AUDIO
  DvmAudio& audio = DvmAudio::Get();
#endif
  while (!dvmWindow.shouldClose()
         && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
  {
    glfwPollEvents();
    double mouseNewX, mouseNewY;
    glfwGetCursorPos(window, &mouseNewX, &mouseNewY);

#ifdef AUDIO
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
      audio.playFromFile(AUDIO_FILE_PATH);
    }
#endif
    glm::vec2 mouseDelta {mouseNewX - mouseInitX, mouseNewY - mouseInitY};
    mouseInitX = mouseNewX;
    mouseInitY = mouseNewY;

    auto newTime = std::chrono::high_resolution_clock::now();

    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(
            newTime - currentTime)
            .count();
    currentTime = newTime;

    if (!dvmGui.getUIVisibility()) {
      cameraController.moveInPlaneXZ(
          window, frameTime, viewerObject, mouseDelta, 0.1f);
      camera.setViewYXZ(viewerObject.transform.translation,
                        viewerObject.transform.rotation);
    }

    float aspect = dvmRenderer.getAspectRatio();

    camera.setPerspectiveProjection(glm::radians(50.f), aspect, .06f, 100.f);

    if (auto commandBuffer = dvmRenderer.beginFrame()) {
      int frameIndex = dvmRenderer.getCurrentFrameIndex();

      FrameInfo frameInfo {frameIndex,
                           frameTime,
                           commandBuffer,
                           camera,
                           globalDescriptorSets[frameIndex],
                           gameObjects};
      GlobalUbo ubo {};

      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.inverseView = camera.getInverseView();
      ubo.time = time++;
      pointLightSystem.update(frameInfo, ubo);

      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      dvmRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo);
      pointLightSystem.render(frameInfo);
      dvmGui.render(frameTime, commandBuffer);
      dvmRenderer.endSwapChainRenderPass(commandBuffer);
      dvmRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(dvmDevice.device());
}

void DvmApp::loadGameObjects()
{
  std::shared_ptr<DvmModel> model =
      DvmModel::createModelFromFile(dvmDevice, "models/flat_vase.obj");

  auto vase = DvmGameObject::createGameObject();
  vase.model = model;
  vase.transform.translation = {
      -0.5f,
      .5f,
      .0f,
  };
  vase.transform.scale = {1.5f, 1.5f, 1.5f};
  gameObjects.emplace(vase.getId(), std::move(vase));

  std::shared_ptr<DvmModel> smoothVaseModel =
      DvmModel::createModelFromFile(dvmDevice, "models/smooth_vase.obj");

  auto smoothVase = DvmGameObject::createGameObject();
  smoothVase.model = smoothVaseModel;
  smoothVase.transform.translation = {
      0.5f,
      .5f,
      .0f,
  };
  smoothVase.transform.scale = {1.5f, 1.5f, 1.5f};
  gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

  std::shared_ptr<DvmModel> suzanneModel =
      DvmModel::createModelFromFile(dvmDevice, "models/suzanne.obj");

  auto suzanne = DvmGameObject::createGameObject();
  suzanne.model = suzanneModel;
  suzanne.transform.translation = {
      0.f,
      -1.5f,
      .0f,
  };
  suzanne.transform.scale = {0.5f, 0.5f, 0.5f};
  gameObjects.emplace(suzanne.getId(), std::move(suzanne));

  std::shared_ptr<DvmModel> floorModel =
      DvmModel::createModelFromFile(dvmDevice, "models/quad.obj");

  auto floorObject = DvmGameObject::createGameObject();
  floorObject.model = floorModel;
  floorObject.transform.translation = {
      .0f,
      .5f,
      .0f,
  };
  floorObject.transform.scale = {5.f, 1.f, 5.f};
  gameObjects.emplace(floorObject.getId(), std::move(floorObject));
  std::vector<glm::vec3> lightColors {
      {1.f, .1f, .1f},
      {.1f, .1f, 1.f},
      {.1f, 1.f, .1f},
      {1.f, 1.f, .1f},
      {.1f, 1.f, 1.f},
      {1.f, 1.f, 1.f},
  };

  for (int i = 0; i < lightColors.size(); i++) {
    auto pointLight = DvmGameObject::createPointLight(0.2f);
    pointLight.color = lightColors[i];
    auto rotateLight =
        glm::rotate(glm::mat4(1.f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    {0.f, -1.f, 0.f});
    pointLight.transform.translation =
        glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
    gameObjects.emplace(pointLight.getId(), std::move(pointLight));
  }
}
}  // namespace dvm
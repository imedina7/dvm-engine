#include "dvm_app.hpp"
#include "dvm_buffer.hpp"
#include "dvm_camera.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_swap_chain.hpp"
#include "keyboard_movement_controller.hpp"
#include <iostream>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <chrono>
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

  Texture texture = Texture(dvmDevice, "../textures/diffuse_bake.png");
  VkDescriptorImageInfo imageInfo = texture.getDescriptorImageInfo();

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

  GLFWwindow* window = dvmWindow.getGLFWwindow();

  auto currentTime = std::chrono::high_resolution_clock::now();

  double mouseInitX, mouseInitY;
  glfwGetCursorPos(window, &mouseInitX, &mouseInitY);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  scene.load();

  entt::registry& registry = scene.getRegistry();

  DvmGUI gui {};
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

    gui.checkUIToggle(mouseDelta);

    auto newTime = std::chrono::high_resolution_clock::now();

    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(
            newTime - currentTime)
            .count();
    currentTime = newTime;

    DvmCamera& camera = scene.getCamera();

    scene.update(frameTime, mouseDelta);

    if (auto commandBuffer = dvmRenderer.beginFrame()) {
      int frameIndex = dvmRenderer.getCurrentFrameIndex();

      FrameInfo frameInfo {frameIndex,
                           frameTime,
                           commandBuffer,
                           camera,
                           globalDescriptorSets[frameIndex],
                           registry};
      GlobalUbo ubo {};

      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.inverseView = camera.getInverseView();
      pointLightSystem.update(frameInfo, ubo);

      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      dvmRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo);
      pointLightSystem.render(frameInfo);
      gui.update(frameTime, commandBuffer);
      dvmRenderer.endSwapChainRenderPass(commandBuffer);
      dvmRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(dvmDevice.device());
}
}  // namespace dvm
#include "dvm_app.hpp"
#include "dvm_camera.hpp"
#include "dvm_frame_info.hpp"
#include <iostream>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <vulkan/vulkan_core.h>

#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace dvm
{
void DvmApp::run()
{
  SimpleRenderSystem simpleRenderSystem {dvmDevice, dvmWindow};
  DvmRenderer& dvmRenderer = simpleRenderSystem.getRenderer();

  PointLightSystem pointLightSystem {
          dvmDevice, dvmRenderer, simpleRenderSystem.getGlobalSetLayout()};

  GLFWwindow* window = dvmWindow.getGLFWwindow();

  auto currentTime = std::chrono::high_resolution_clock::now();

  double mouseInitX, mouseInitY;
  glfwGetCursorPos(window, &mouseInitX, &mouseInitY);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  m_Scene.load();

  entt::registry& registry = m_Scene.getRegistry();

  DvmGUI gui {dvmRenderer};

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

    gui.checkUIToggle();

    auto newTime = std::chrono::high_resolution_clock::now();

    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(
            newTime - currentTime)
            .count();
    currentTime = newTime;

    GlobalUbo ubo = m_Scene.update(frameTime, mouseDelta, !gui.getUIVisibility(), dvmRenderer.getAspectRatio());

    if (auto commandBuffer = dvmRenderer.beginFrame()) {
      int frameIndex = dvmRenderer.getCurrentFrameIndex();

      FrameInfo frameInfo {frameIndex,
                           frameTime,
                           commandBuffer,
                           m_Scene};

      simpleRenderSystem.update(frameInfo, ubo);
      pointLightSystem.update(frameInfo, ubo);

      dvmRenderer.beginSwapChainRenderPass(commandBuffer);

      simpleRenderSystem.render(frameInfo);
      pointLightSystem.render(frameInfo);
      gui.render(frameInfo);

      dvmRenderer.endSwapChainRenderPass(commandBuffer);
      dvmRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(dvmDevice.device());
}
}  // namespace dvm
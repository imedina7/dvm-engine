#include "dvm_app.hpp"
#include "dvm_gui.hpp"
#include "dvm_frame_info.hpp"
#include <chrono>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "systems/deferred_render_system.hpp"
#include "systems/point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cmath>
#include <glm/glm.hpp>

namespace dvm
{
void DvmApp::run()
{
  DeferredRenderSystem deferredRenderSystem {m_dvmDevice, m_dvmWindow};
  DvmRenderer& dvmRenderer = deferredRenderSystem.getRenderer();

  PointLightSystem pointLightSystem {
          m_dvmDevice, dvmRenderer, deferredRenderSystem.getGlobalSetLayout()};

  GLFWwindow* window = m_dvmWindow.getGLFWwindow();

  auto currentTime = std::chrono::high_resolution_clock::now();

  double mouseInitX, mouseInitY;
  glfwGetCursorPos(window, &mouseInitX, &mouseInitY);

  if (glfwRawMouseMotionSupported() == GLFW_TRUE) {
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  m_scene.load();

  entt::registry& registry = m_scene.getRegistry();

  DvmGUI gui {dvmRenderer};

#ifdef AUDIO
  DvmAudio& audio = DvmAudio::Get();
#endif

  while (!m_dvmWindow.shouldClose()
         && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
  {
    glfwPollEvents();
    double mouseNewX = NAN;
    double mouseNewY = NAN;
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

    GlobalUbo ubo = m_scene.update(frameTime, mouseDelta, !gui.getUIVisibility(), dvmRenderer.getAspectRatio());

    if (auto commandBuffer = dvmRenderer.beginFrame()) {
      int frameIndex = dvmRenderer.getCurrentFrameIndex();

      FrameInfo frameInfo {frameIndex,
                           frameTime,
                           commandBuffer,
                           m_scene};

      pointLightSystem.update(frameInfo, ubo);
      deferredRenderSystem.update(frameInfo, ubo);

      dvmRenderer.beginSwapChainRenderPass(commandBuffer);

      deferredRenderSystem.render(frameInfo);
      pointLightSystem.render(frameInfo);
      gui.render(frameInfo);

      dvmRenderer.endSwapChainRenderPass(commandBuffer);
      dvmRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(m_dvmDevice.device());
}
}  // namespace dvm
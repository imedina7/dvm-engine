#pragma once

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_vulkan.h"
#include "gui/panels/panel.hpp"
#include "gui/panels/outliner.hpp"

#include <vector>

#include <glm/glm.hpp>

#include "dvm_app.hpp"
#include "dvm_device.hpp"
#include "dvm_descriptors.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_renderer.hpp"
#include "dvm_window.hpp"

static void check_vk_result(VkResult err)
{
  if (err == 0)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}

namespace dvm
{
struct GlobalState
{
  gui::OutlinerState outlinerState;
};
class DvmGUI
{
public:
  DvmGUI(DvmRenderer& dvmRenderer);
  ~DvmGUI()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void render(FrameInfo& frameInfo);
  const bool toggleUI()
  {
    uiVisible = !uiVisible;
    return uiVisible;
  };

  const bool getUIVisibility() const { return uiVisible; };
  const GlobalState& getState() const { return uiState; };

  void checkUIToggle();

private:
  void initStyle();
  void beginFrame();
  void renderPanels(VkCommandBuffer commandBuffer);
  void endFrame();

private:
  GLFWwindow* glfwWindow;
  GlobalState uiState;
  DvmRenderer& dvmRenderer;
  std::vector<gui::Panel*> panels;
  std::unique_ptr<DvmDescriptorPool> descriptorPool;
  bool uiVisible = false;
  float frameTime = 1.f;
  glm::vec2 monitorScale {1.f};

private:  // FONTS
  ImFont* robotoMedium;
  ImFont* doppioOne;
};
}  // namespace dvm
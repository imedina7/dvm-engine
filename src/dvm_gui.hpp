#pragma once

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_vulkan.h"
#include "gui/panels/panel.hpp"
#include "gui/panels/outliner.hpp"

#include <vector>

#include <glm/glm.hpp>

#include "dvm_device.hpp"
#include "dvm_descriptors.hpp"
#include "dvm_app.hpp"
#include "dvm_window.hpp"
#include "dvm_renderer.hpp"

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
  DvmGUI();
  ~DvmGUI()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void update(float dt, VkCommandBuffer command_buffer);
  const bool toggleUI()
  {
    uiVisible = !uiVisible;
    return uiVisible;
  };

  const bool getUIVisibility() const { return uiVisible; };
  const GlobalState& getState() const { return uiState; };

  void checkUIToggle(glm::vec2 mouseDelta);

private:
  GLFWwindow* glfwWindow;
  GlobalState uiState;
  std::vector<gui::Panel*> panelWindows;
  std::unique_ptr<DvmDescriptorPool> descriptorPool;
  bool uiVisible = false;
};
}  // namespace dvm
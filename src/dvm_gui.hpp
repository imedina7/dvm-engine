#pragma once

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_vulkan.h"

#include<glm/glm.hpp>

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
    ui_visible = !ui_visible;
    return ui_visible;
  };

  const bool getUIVisibility() { return ui_visible; };

  void checkUIToggle(glm::vec2 mouseDelta);

private:
  GLFWwindow* glfwWindow;
  std::unique_ptr<DvmDescriptorPool> descriptorPool;
  bool show_demo_window = true;
  bool ui_visible = false;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};
}  // namespace dvm
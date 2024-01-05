#pragma once

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include <vulkan/vulkan.h>

#include "dvm_device.hpp"

namespace dvm
{
class DvmGUI
{
public:
  DvmGUI(GLFWwindow* window, DvmDevice& device, VkRenderPass renderPass);
  ~DvmGUI();

  void NewFrame();

private:
  //ImGuiIO& io {};
  ImGui_ImplVulkanH_Window g_MainWindowData;
};
}  // namespace dvm
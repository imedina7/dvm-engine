#pragma once

#include "gui/imgui_vulkan_renderer.h"

#include "dvm_device.hpp"

namespace dvm
{
class DvmGUI
{
public:
  DvmGUI(GLFWwindow* window,
         VkDescriptorPool descriptorPool,
         DvmDevice& device,
         VkRenderPass renderPass);

  void update();

private:
  // ImGuiIO& io;
  ImGui_ImplVulkanH_Window g_MainWindowData;
};
}  // namespace dvm
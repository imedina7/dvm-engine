#pragma once

#include <imgui/backends/imgui_impl_glfw.cpp>
#include <imgui/backends/imgui_impl_vulkan.cpp>

#include <vulkan/vulkan.h>

#include "dvm_device.hpp"

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
  DvmGUI(GLFWwindow* window,
         VkDescriptorPool descriptorPool,
         DvmDevice& device,
         VkRenderPass renderPass);
  ~DvmGUI();

  void NewFrame();

private:
  ImGuiIO& io;
  ImGui_ImplVulkanH_Window g_MainWindowData;
};
}  // namespace dvm
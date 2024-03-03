#include "dvm_gui.hpp"

namespace dvm
{
DvmGUI::DvmGUI(GLFWwindow* window,
               VkDescriptorPool descriptorPool,
               DvmDevice& dvmDevice,
               VkRenderPass renderPass)
{
  ImGui_ImplVulkan_InitInfo initInfo {};
  initInfo.Instance = dvmDevice.getInstance();
  initInfo.Device = dvmDevice.device();
  initInfo.PhysicalDevice = dvmDevice.getPhysicalDevice();
  initInfo.QueueFamily = dvmDevice.findPhysicalQueueFamilies().graphicsFamily;
  initInfo.Queue = dvmDevice.graphicsQueue();
  initInfo.DescriptorPool = descriptorPool;
  initInfo.Subpass = 0;
  initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  // initInfo.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&initInfo, renderPass);
  // ImGui_ImplGlfw_InitForVulkan(window, false);
}

void DvmGUI::update() {
  // ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
  // IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplGlfw_InitForXXX()?");

  // ImGuiIO& io = ImGui::GetIO();

  // // Setup display size (every frame to accommodate for window resizing)
  // int w, h;
  // int display_w, display_h;
  // glfwGetWindowSize(bd->Window, &w, &h);
  // glfwGetFramebufferSize(bd->Window, &display_w, &display_h);
  // io.DisplaySize = ImVec2((float)w, (float)h);
  // if (w > 0 && h > 0)
  //   io.DisplayFramebufferScale =
  //       ImVec2((float)display_w / (float)w, (float)display_h / (float)h);
  // if (bd->WantUpdateMonitors)
  //   ImGui_ImplGlfw_UpdateMonitors();

  // // Setup time step
  // double current_time = glfwGetTime();
  // io.DeltaTime =
  //     bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
  // bd->Time = current_time;

  // ImGui_ImplGlfw_UpdateMouseData();
  // ImGui_ImplGlfw_UpdateMouseCursor();

  // // Update game controllers (if enabled and available)
  // ImGui_ImplGlfw_UpdateGamepads();
}
}  // namespace dvm
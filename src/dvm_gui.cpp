#include "dvm_gui.hpp"

namespace dvm
{
  DvmGUI::DvmGUI(GLFWwindow* window,
                 DvmDevice& dvmDevice,
                 VkRenderPass renderPass)
  {
    //io = ImGui::GetIO();
    /*ImGui_ImplVulkan_InitInfo initInfo {};
    initInfo.Instance = dvmDevice.getInstance();
    initInfo.Device = dvmDevice.device();
    initInfo.PhysicalDevice = dvmDevice.getPhysicalDevice();
    initInfo.QueueFamily = g_QueueFamily;
    initInfo.Queue = g_Queue;
    initInfo.PipelineCache = g_PipelineCache;
    initInfo.DescriptorPool = g_DescriptorPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = g_MinImageCount;
    initInfo.ImageCount = wd->ImageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = g_Allocator;
    initInfo.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&initInfo, renderPass);*/
  }

  void DvmGUI::NewFrame()
  {
    //ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
    //IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplGlfw_InitForXXX()?");

    //ImGuiIO& io = ImGui::GetIO();

    //// Setup display size (every frame to accommodate for window resizing)
    //int w, h;
    //int display_w, display_h;
    //glfwGetWindowSize(bd->Window, &w, &h);
    //glfwGetFramebufferSize(bd->Window, &display_w, &display_h);
    //io.DisplaySize = ImVec2((float)w, (float)h);
    //if (w > 0 && h > 0)
    //  io.DisplayFramebufferScale =
    //      ImVec2((float)display_w / (float)w, (float)display_h / (float)h);
    //if (bd->WantUpdateMonitors)
    //  ImGui_ImplGlfw_UpdateMonitors();

    //// Setup time step
    //double current_time = glfwGetTime();
    //io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time)
    //                              : (float)(1.0f / 60.0f);
    //bd->Time = current_time;

    //ImGui_ImplGlfw_UpdateMouseData();
    //ImGui_ImplGlfw_UpdateMouseCursor();

    //// Update game controllers (if enabled and available)
    //ImGui_ImplGlfw_UpdateGamepads();
  }
  }  // namespace dvm
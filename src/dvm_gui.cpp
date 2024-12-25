#include "dvm_gui.hpp"

namespace dvm
{
DvmGUI::DvmGUI()
{
  DvmApp& app = DvmApp::getInstance();
  glfwWindow = app.getWindow().getGLFWwindow();
  DvmDevice& dvmDevice = app.getDevice();
  DvmWindow& dvmWindow = app.getWindow();

  descriptorPool =
      DvmDescriptorPool::Builder(dvmDevice)
          .setMaxSets(11000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
          .addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
          .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
          .build();

  DvmRenderer& dvmRenderer = app.getRenderer();

  uiState.outlinerState.isVisible = true;

  panelWindows.emplace_back(new gui::Outliner(uiState.outlinerState));

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
// Gamepad
// Controls
#ifdef GUI_DOCKING
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable
  // Multi
  //     - Viewport
  // / Platform Windows
  io.ConfigViewportsNoAutoMerge = true;
  io.ConfigViewportsNoTaskBarIcon = true;
#endif

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
#ifdef GUI_DOCKING
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
#endif
  ImGui_ImplGlfw_InitForVulkan(dvmWindow.getGLFWwindow(), true);

  ImGui_ImplVulkan_InitInfo initInfo {};
  initInfo.Instance = dvmDevice.getInstance();
  initInfo.Device = dvmDevice.device();
  initInfo.PhysicalDevice = dvmDevice.getPhysicalDevice();
  initInfo.QueueFamily = dvmDevice.findPhysicalQueueFamilies().graphicsFamily;
  initInfo.Queue = dvmDevice.graphicsQueue();
  initInfo.DescriptorPool = descriptorPool->getDescriptorPool();
  initInfo.CheckVkResultFn = check_vk_result;
  initInfo.Subpass = 0;
  initInfo.Allocator = nullptr;
  initInfo.MinImageCount = 2;
  initInfo.ImageCount = 2;
  initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  initInfo.RenderPass = dvmRenderer.getSwapChainRenderPass();

  ImGui_ImplVulkan_Init(&initInfo);

  // Upload Fonts
  {
    ImGui_ImplVulkan_CreateFontsTexture();

    ImGui_ImplVulkan_DestroyFontsTexture();
  }
}

void DvmGUI::update(float dt, VkCommandBuffer command_buffer)
{
  DvmApp& app = DvmApp::getInstance();
  DvmDevice& dvmDevice = app.getDevice();

  ImGuiIO& io = ImGui::GetIO();
  int width, height;
  glfwGetFramebufferSize(glfwWindow, &width, &height);

  io.DisplaySize =
      ImVec2(static_cast<float>(width), static_cast<float>(height));
  io.DeltaTime = dt;
  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplVulkan_NewFrame();
  ImGui::NewFrame();

  for (auto panel : panelWindows) {
    if (panel->isVisible())
      panel->draw();
  }
  ImGui::Render();

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);

#ifdef GUI_DOCKING
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
#endif
  ImGui::EndFrame();
}

void DvmGUI::checkUIToggle(glm::vec2 mouseDelta)
{
  if (glfwGetKey(glfwWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
    if (glm::length(glm::normalize(mouseDelta)) > .7f)
      return;
    if (toggleUI()) {
      glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
  }
}
}  // namespace dvm
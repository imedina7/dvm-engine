#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace dvm
{

class DvmWindow
{
public:
  DvmWindow(int w, int h, std::string name);
  ~DvmWindow();

  DvmWindow(const DvmWindow&) = delete;
  DvmWindow& operator=(const DvmWindow&) = delete;

  bool wasWindowResized() const { return framebufferResized; }
  void resetWindowResized() { framebufferResized = false; }
  bool shouldClose() { return glfwWindowShouldClose(window); }

  GLFWwindow* getGLFWwindow() const { return window; };
  VkExtent2D getExtent()
  {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }
  void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
  GLFWmonitor** getMonitors() { return monitors; };
  GLFWmonitor* getPrimaryMonitor() { return monitors[0]; };
  const int getMonitorCount() const { return monitorCount; };

  std::pair<float, float> getMonitorDPI();
  std::pair<float, float> getMonitorDPI(GLFWmonitor* monitor);

private:
  static void framebufferResizedCallback(GLFWwindow* window,
                                         int width,
                                         int height);
  void initWindow();

  bool framebufferResized;
  int width;
  int height;

  std::string windowName;
  GLFWwindow* window;
  VkSurfaceKHR* surface;
  GLFWmonitor** monitors;
  int monitorCount;
};
}  // namespace dvm
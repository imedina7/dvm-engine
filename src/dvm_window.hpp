#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace dvm {

class DvmWindow {
public:
  DvmWindow(int w, int h, std::string name);
  ~DvmWindow();

  DvmWindow(const DvmWindow &) = delete;
  DvmWindow &operator=(const DvmWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }

  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }
  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  void initWindow();

  const int width;
  const int height;

  std::string windowName;
  GLFWwindow *window;
};
} // namespace dvm
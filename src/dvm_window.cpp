#include "dvm_window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace dvm
{
DvmWindow::DvmWindow(int w, int h, std::string name)
    : width {w}
    , height {h}
    , windowName {name}
{
  initWindow();
}

DvmWindow::~DvmWindow()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}

void DvmWindow::initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
  monitors = glfwGetMonitors(&monitorCount);
}

void DvmWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* _surface)
{
  if (glfwCreateWindowSurface(instance, window, nullptr, _surface)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create window surface");
  }
  surface = _surface;
}

void DvmWindow::framebufferResizedCallback(GLFWwindow* window,
                                           int width,
                                           int height)
{
  auto dvmWindow =
      reinterpret_cast<DvmWindow*>(glfwGetWindowUserPointer(window));
  dvmWindow->framebufferResized = true;
  dvmWindow->width = width;
  dvmWindow->height = height;
}

glm::vec2 DvmWindow::getMonitorDPI()
{
  float xScale, yScale;
  glfwGetMonitorContentScale(getPrimaryMonitor(), &xScale, &yScale);
  return glm::vec2(xScale, yScale);
}
glm::vec2 DvmWindow::getMonitorDPI(GLFWmonitor* monitor)
{
  float xScale, yScale;
  glfwGetMonitorContentScale(monitor, &xScale, &yScale);
  return glm::vec2(xScale, yScale);
}
}  // namespace dvm
#pragma once

#include "dvm_window.hpp"

// std lib headers
#include <string>
#include <vector>

namespace dvm
{

struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
  uint32_t graphicsFamily;
  uint32_t presentFamily;
  uint32_t computeFamily;
  bool graphicsFamilyHasValue = false;
  bool presentFamilyHasValue = false;
  bool computeFamilyHasValue = false;
  bool isComplete()
  {
    return graphicsFamilyHasValue && presentFamilyHasValue
        && computeFamilyHasValue;
  }
};

class DvmDevice
{
public:
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  enum class CommandType
  {
    GRAPHICS = 0,
    COMPUTE
  };

  DvmDevice(DvmWindow& window);
  ~DvmDevice();

  // Not copyable or movable
  DvmDevice(const DvmDevice&) = delete;
  DvmDevice& operator=(const DvmDevice&) = delete;
  DvmDevice(DvmDevice&&) = delete;
  DvmDevice& operator=(DvmDevice&&) = delete;

  VkInstance getInstance() { return instance; }
  VkCommandPool getCommandPool() { return commandPool; }
  VkCommandPool getComputeCommandPool() { return computeCommandPool; }
  VkDevice device() { return device_; }
  VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
  VkSurfaceKHR surface() { return surface_; }
  VkQueue graphicsQueue() { return graphicsQueue_; }
  VkQueue computeQueue() { return computeQueue_; }
  VkQueue presentQueue() { return presentQueue_; }

  SwapChainSupportDetails getSwapChainSupport()
  {
    return querySwapChainSupport(physicalDevice);
  }
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  QueueFamilyIndices findPhysicalQueueFamilies()
  {
    return findQueueFamilies(physicalDevice);
  }
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

  // Buffer Helper Functions
  void createBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);
  VkCommandBuffer beginSingleTimeCommands(
      CommandType commandType = CommandType::GRAPHICS);
  void endSingleTimeCommands(VkCommandBuffer commandBuffer,
                             CommandType commandType = CommandType::GRAPHICS);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void copyBufferToImage(VkBuffer buffer,
                         VkImage image,
                         uint32_t width,
                         uint32_t height,
                         uint32_t layerCount);

  void createImageWithInfo(const VkImageCreateInfo& imageInfo,
                           VkMemoryPropertyFlags properties,
                           VkImage& image,
                           VkDeviceMemory& imageMemory);

  VkPhysicalDeviceProperties properties;

private:
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();

  // helper functions
  bool isDeviceSuitable(VkPhysicalDevice device);
  std::vector<const char*> getRequiredExtensions();
  bool checkValidationLayerSupport();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void hasGflwRequiredInstanceExtensions();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  DvmWindow& window;
  VkCommandPool commandPool;
  VkCommandPool computeCommandPool;

  VkDevice device_;
  VkSurfaceKHR surface_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  VkQueue computeQueue_;

  const std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  const std::vector<const char*> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace dvm
#include "dvm_app.hpp"
#include "dvm_swap_chain.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace dvm {
DvmApp::DvmApp() {
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}
DvmApp::~DvmApp() {
  vkDestroyPipelineLayout(dvmDevice.device(), pipelineLayout, nullptr);
}

void DvmApp::run() {
  while (!dvmWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }
  vkDeviceWaitIdle(dvmDevice.device());
}
void DvmApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(dvmDevice.device(), &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void DvmApp::createPipeline() {
  auto pipelineConfig = DvmPipeline::defaultPipelineConfigInfo(
      dvmSwapChain.width(), dvmSwapChain.height());
  pipelineConfig.renderPass = dvmSwapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  dvmPipeline = std::make_unique<DvmPipeline>(
      dvmDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfig);
}

void DvmApp::createCommandBuffers() {
  commandBuffers.resize(dvmSwapChain.imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  allocInfo.commandPool = dvmDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(dvmDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (int i = 0; i < commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = dvmSwapChain.getRenderPass();
    renderPassInfo.framebuffer = dvmSwapChain.getFrameBuffer(i);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = dvmSwapChain.getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 0.1f};
    clearValues[1].depthStencil = {0.1f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    dvmPipeline->bind(commandBuffers[i]);

    vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);
    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}
void DvmApp::drawFrame() {
  uint32_t imageIndex;
  auto result = dvmSwapChain.acquireNextImage(&imageIndex);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  result = dvmSwapChain.submitCommandBuffers(&commandBuffers[imageIndex],
                                             &imageIndex);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }
}
} // namespace dvm
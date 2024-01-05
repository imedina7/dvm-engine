#include "dvm_app.hpp"
#include "dvm_swap_chain.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace dvm {
struct SimplePushConstantData {
  glm::mat2 transform{1.0f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

DvmApp::DvmApp() {
  loadGameObjects();
}
DvmApp::~DvmApp() {}

void DvmApp::run() {
  SimpleRenderSystem simpleRenderSystem{dvmDevice, dvmRenderer.getSwapChainRenderPass()};
  while (!dvmWindow.shouldClose()) {
    glfwPollEvents();

    if(auto commandBuffer = dvmRenderer.beginFrame()) {
      dvmRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
      dvmRenderer.endSwapChainRenderPass(commandBuffer);
      dvmRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(dvmDevice.device());
}

void DvmApp::loadGameObjects() {
  std::vector<DvmModel::Vertex> vertices {
    {{0.0, -0.5}, {1.0, 0.0, 0.0}},
    {{0.5, 0.5}, {0.0, 1.0, 0.0}},
    {{-0.5, 0}, {0.0, 0.0, 1.0}}
  };

  auto model = std::make_shared<DvmModel>(dvmDevice, vertices);

  auto triangle = DvmGameObject::createGameObject();
  triangle.model = model;
  triangle.color = { .1f, 1.f, .0f};
  triangle.transform2d.translation.x = .2f;
  triangle.transform2d.scale = {2.0f, 0.5f};
  triangle.transform2d.rotation = .25f * glm::two_pi<float>();

  gameObjects.push_back(std::move(triangle));
}
} // namespace dvm
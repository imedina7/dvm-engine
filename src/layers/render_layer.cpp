#include "render_layer.hpp"
#include "../dvm_frame_info.hpp"

namespace dvm
{
void RenderLayer::onAttach()
{
  m_deferredRenderSystem = std::make_unique<DeferredRenderSystem>(
      *m_context.device, *m_context.window);

  m_renderer = &m_deferredRenderSystem->getRenderer();

  m_pointLightSystem = std::make_unique<PointLightSystem>(
      *m_context.device,
      *m_renderer,
      m_deferredRenderSystem->getGlobalSetLayout());

  // Update context with aspect ratio
  m_context.aspectRatio = m_renderer->getAspectRatio();
}

void RenderLayer::onUpdate(float dt)
{
  // Update aspect ratio in context
  m_context.aspectRatio = m_renderer->getAspectRatio();
}

void RenderLayer::onRender()
{
  if (!m_context.activeScene) {
    return;
  }

  // Get UBO from context (set by SceneLayer)
  GlobalUbo& ubo = m_context.currentFrameUbo;

  if (auto commandBuffer = m_renderer->beginFrame()) {
    int frameIndex = m_renderer->getCurrentFrameIndex();

    FrameInfo frameInfo {frameIndex,
                         m_context.deltaTime,
                         commandBuffer,
                         *m_context.activeScene};

    // Update systems
    m_pointLightSystem->update(frameInfo, ubo);
    m_deferredRenderSystem->update(frameInfo, ubo);

    m_renderer->beginSwapChainRenderPass(commandBuffer);

    // Render
    m_deferredRenderSystem->render(frameInfo);
    m_pointLightSystem->render(frameInfo);
    // ImGuiLayer will render after this

    m_renderer->endSwapChainRenderPass(commandBuffer);
    m_renderer->endFrame();
  }
}

void RenderLayer::onEvent(Event& e)
{
  EventDispatcher dispatcher(e);

  dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
    // Handle viewport resize if needed
    m_context.aspectRatio = m_renderer->getAspectRatio();
    return false;
  });
}

void RenderLayer::onDetach()
{
  m_pointLightSystem.reset();
  m_deferredRenderSystem.reset();
  m_renderer = nullptr;
}
}  // namespace dvm

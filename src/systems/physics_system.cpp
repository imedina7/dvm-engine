#include "dvm_app.hpp"
#include "physics_system.hpp"
namespace dvm
{
PhysicsSystem::PhysicsSystem(VkDescriptorSetLayout globalDescriptorSetLayout)
    : dvmDevice {DvmApp::getInstance().getDevice()}
{
  auto physicsLayout =
      DvmDescriptorSetLayout::Builder(dvmDevice)
          .addBinding(
              0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
          .build();
  std::vector<VkDescriptorSetLayout> layouts {
      globalDescriptorSetLayout, physicsLayout->getDescriptorSetLayout()};

  createPipelineLayout(layouts);
  createPipeline();
}
void PhysicsSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
{
  dvmPipeline->bind(frameInfo.commandBuffer);

  auto view = frameInfo.registry.view<PhysicsMaterialComponent,
                                      RigidBodyComponent,
                                      TransformComponent,
                                      CollisionObject>();

  for (auto&& [entity, material, rigidBody, transform, collision] : view.each())
  {
  }
}

void PhysicsSystem::createPipelineLayout(
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
  VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount =
      static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

  if (vkCreatePipelineLayout(
          dvmDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void PhysicsSystem::createPipeline()
{
  PipelineConfigInfo pipelineConfig {};
  pipelineConfig.pipelineLayout = pipelineLayout;
  std::vector<std::string> shaders = {"shaders/physics.comp.spv"};

  dvmPipeline = std::make_unique<DvmPipeline>(
      dvmDevice, shaders, pipelineConfig, DvmPipeline::PipelineType::COMPUTE);
}
}  // namespace dvm
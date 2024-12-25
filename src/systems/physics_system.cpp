#include "dvm_app.hpp"
#include "dvm_swap_chain.hpp"
#include "physics_system.hpp"
namespace dvm
{
PhysicsSystem::PhysicsSystem()
    : dvmDevice {DvmApp::getInstance().getDevice()}
{
  createPipelineLayout();
  createPipeline();
  createSyncObjects();
  createCommandBuffers();
  createDescriptorPool();
  createStorageBuffers();
  createUniformBuffers();
  createDescriptorSets();
}
PhysicsSystem::~PhysicsSystem()
{
  freeCommandBuffers();
  vkDestroyPipelineLayout(dvmDevice.device(), pipelineLayout, nullptr);
}

void PhysicsSystem::update(FrameInfo& frameInfo)
{
  const DvmCamera& camera = frameInfo.camera;
  int currentFrame = frameInfo.frameIndex;

  VkCommandBuffer commandBuffer = commandBuffers.at(currentFrame);

  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  dvmPipeline->bind(commandBuffer);

  PhysicsUbo physicsUbo {};
  physicsUbo.projection = camera.getProjection();
  physicsUbo.view = camera.getView();
  physicsUbo.inverseView = camera.getInverseView();
  physicsUbo.deltaTime = frameInfo.frameTime;

  auto view =
      frameInfo.registry
          .view<PhysicsMaterial, RigidBodyComponent, TransformComponent>();
  PhysicsStorage storage {};
  std::array<RigidBody, MAX_PHYSICS_OBJECTS> bodies;
  uint32_t numBodies = 0;

  for (const auto&& [entity, material, rigidBody, transform] : view.each()) {
    if (numBodies == MAX_PHYSICS_OBJECTS)
      break;
    bodies[numBodies].velocity = glm::vec4(rigidBody.velocity, 0.f);
    bodies[numBodies].mass = rigidBody.mass;
    bodies[numBodies].drag = rigidBody.drag;
    bodies[numBodies].useGravity = rigidBody.useGravity;
    bodies[numBodies].position = glm::vec4(transform.translation, 0.f);
    bodies[numBodies].acceleration = glm::vec4(rigidBody.acceleration, 0.f);
    bodies[numBodies].modelMatrix = transform.mat4();
    numBodies++;
  }

  physicsUbo.numBodies = numBodies;

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &computeFinishedSemaphores[currentFrame];
  submitInfo.signalSemaphoreCount = 0;

  if (vkQueueSubmit(dvmDevice.computeQueue(), 1, &submitInfo, nullptr)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to submit compute command buffer!");
  };
}

void PhysicsSystem::createStorageBuffers()
{
  for (int i = 0; i < storageBuffers.size(); i++) {
    storageBuffers[i] =
        std::make_unique<DvmBuffer>(dvmDevice,
                                    sizeof(PhysicsStorage),
                                    1,
                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    storageBuffers[i]->map();
  }
}

void PhysicsSystem::createUniformBuffers()
{
  for (int i = 0; i < uniformBuffers.size(); i++) {
    uniformBuffers[i] =
        std::make_unique<DvmBuffer>(dvmDevice,
                                    sizeof(PhysicsStorage),
                                    1,
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniformBuffers[i]->map();
  }
}

void PhysicsSystem::createDescriptorSets()
{
  for (int i = 0; i < descriptorSets.size(); i++) {
    auto uniformBufferInfo = uniformBuffers[i]->descriptorInfo();
    auto storageBufferInfo = storageBuffers[i]->descriptorInfo();
    DvmDescriptorWriter(*setLayout, *descriptorPool)
        .writeBuffer(0, &uniformBufferInfo)
        .writeBuffer(1, &storageBufferInfo)
        .build(descriptorSets[i]);
  }
}

void PhysicsSystem::createDescriptorPool()
{
  descriptorPool = DvmDescriptorPool::Builder(dvmDevice)
                       .setMaxSets(DvmSwapChain::MAX_FRAMES_IN_FLIGHT * 2)
                       .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                    DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                       .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                    DvmSwapChain::MAX_FRAMES_IN_FLIGHT)
                       .build();
}

void PhysicsSystem::createPipelineLayout()
{
  setLayout =
      DvmDescriptorSetLayout::Builder(dvmDevice)
          .addBinding(
              0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
          .addBinding(
              1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
          .build();

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts {
      setLayout->getDescriptorSetLayout()};

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
void PhysicsSystem::createCommandBuffers()
{
  commandBuffers.resize(DvmSwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = dvmDevice.getComputeCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(
          dvmDevice.device(), &allocInfo, commandBuffers.data())
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to allocate compute command buffers!");
  }
}

void PhysicsSystem::freeCommandBuffers()
{
  vkFreeCommandBuffers(dvmDevice.device(),
                       dvmDevice.getComputeCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

void PhysicsSystem::createSyncObjects()
{
  VkSemaphoreCreateInfo semaphoreInfo {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < DvmSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(dvmDevice.device(),
                          &semaphoreInfo,
                          nullptr,
                          &computeFinishedSemaphores[i])
            != VK_SUCCESS
        || vkCreateFence(dvmDevice.device(),
                         &fenceInfo,
                         nullptr,
                         &computeInFlightFences[i])
            != VK_SUCCESS)
    {
      throw std::runtime_error(
          "failed to create compute synchronization objects for a frame!");
    }
  }
}
}  // namespace dvm
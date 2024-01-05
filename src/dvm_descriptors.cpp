#include "dvm_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace dvm
{

// *************** Descriptor Set Layout Builder *********************

DvmDescriptorSetLayout::Builder& DvmDescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count)
{
  assert(bindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding {};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  bindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<DvmDescriptorSetLayout> DvmDescriptorSetLayout::Builder::build()
    const
{
  return std::make_unique<DvmDescriptorSetLayout>(dvmDevice, bindings);
}

// *************** Descriptor Set Layout *********************

DvmDescriptorSetLayout::DvmDescriptorSetLayout(
    DvmDevice& dvmDevice,
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : dvmDevice {dvmDevice}
    , bindings {bindings}
{
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings {};
  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo {};
  descriptorSetLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount =
      static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

  if (vkCreateDescriptorSetLayout(dvmDevice.device(),
                                  &descriptorSetLayoutInfo,
                                  nullptr,
                                  &descriptorSetLayout)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

DvmDescriptorSetLayout::~DvmDescriptorSetLayout()
{
  vkDestroyDescriptorSetLayout(
      dvmDevice.device(), descriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

DvmDescriptorPool::Builder& DvmDescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32_t count)
{
  poolSizes.push_back({descriptorType, count});
  return *this;
}

DvmDescriptorPool::Builder& DvmDescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags)
{
  poolFlags = flags;
  return *this;
}
DvmDescriptorPool::Builder& DvmDescriptorPool::Builder::setMaxSets(
    uint32_t count)
{
  maxSets = count;
  return *this;
}

std::unique_ptr<DvmDescriptorPool> DvmDescriptorPool::Builder::build() const
{
  return std::make_unique<DvmDescriptorPool>(
      dvmDevice, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

DvmDescriptorPool::DvmDescriptorPool(
    DvmDevice& dvmDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize>& poolSizes)
    : dvmDevice {dvmDevice}
{
  VkDescriptorPoolCreateInfo descriptorPoolInfo {};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  descriptorPoolInfo.flags = poolFlags;

  if (vkCreateDescriptorPool(
          dvmDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

DvmDescriptorPool::~DvmDescriptorPool()
{
  vkDestroyDescriptorPool(dvmDevice.device(), descriptorPool, nullptr);
}

bool DvmDescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout,
    VkDescriptorSet& descriptor) const
{
  VkDescriptorSetAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.pSetLayouts = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this
  // case, and builds a new pool whenever an old pool fills up. But this is
  // beyond our current scope
  if (vkAllocateDescriptorSets(dvmDevice.device(), &allocInfo, &descriptor)
      != VK_SUCCESS)
  {
    return false;
  }
  return true;
}

void DvmDescriptorPool::freeDescriptors(
    std::vector<VkDescriptorSet>& descriptors) const
{
  vkFreeDescriptorSets(dvmDevice.device(),
                       descriptorPool,
                       static_cast<uint32_t>(descriptors.size()),
                       descriptors.data());
}

void DvmDescriptorPool::resetPool()
{
  vkResetDescriptorPool(dvmDevice.device(), descriptorPool, 0);
}

// *************** Descriptor Writer *********************

DvmDescriptorWriter::DvmDescriptorWriter(DvmDescriptorSetLayout& setLayout,
                                         DvmDescriptorPool& pool)
    : setLayout {setLayout}
    , pool {pool}
{
}

DvmDescriptorWriter& DvmDescriptorWriter::writeBuffer(
    uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
  assert(setLayout.bindings.count(binding) == 1
         && "Layout does not contain specified binding");

  auto& bindingDescription = setLayout.bindings[binding];

  assert(bindingDescription.descriptorCount == 1
         && "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write {};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

DvmDescriptorWriter& DvmDescriptorWriter::writeImage(
    uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
  assert(setLayout.bindings.count(binding) == 1
         && "Layout does not contain specified binding");

  auto& bindingDescription = setLayout.bindings[binding];

  assert(bindingDescription.descriptorCount == 1
         && "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write {};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

bool DvmDescriptorWriter::build(VkDescriptorSet& set)
{
  bool success =
      pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void DvmDescriptorWriter::overwrite(VkDescriptorSet& set)
{
  for (auto& write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(
      pool.dvmDevice.device(), writes.size(), writes.data(), 0, nullptr);
}

}  // namespace dvm
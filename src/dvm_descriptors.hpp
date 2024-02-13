#pragma once

#include "dvm_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace dvm
{

class DvmDescriptorSetLayout
{
public:
  class Builder
  {
  public:
    Builder(DvmDevice& dvmDevice)
        : dvmDevice {dvmDevice}
    {
    }

    Builder& addBinding(uint32_t binding,
                        VkDescriptorType descriptorType,
                        VkShaderStageFlags stageFlags,
                        uint32_t count = 1);
    std::unique_ptr<DvmDescriptorSetLayout> build() const;

  private:
    DvmDevice& dvmDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings {};
  };

  DvmDescriptorSetLayout(
      DvmDevice& dvmDevice,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~DvmDescriptorSetLayout();
  DvmDescriptorSetLayout(const DvmDescriptorSetLayout&) = delete;
  DvmDescriptorSetLayout& operator=(const DvmDescriptorSetLayout&) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const
  {
    return descriptorSetLayout;
  }

private:
  DvmDevice& dvmDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class DvmDescriptorWriter;
};

class DvmDescriptorPool
{
public:
  class Builder
  {
  public:
    Builder(DvmDevice& dvmDevice)
        : dvmDevice {dvmDevice}
    {
    }

    Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder& setMaxSets(uint32_t count);
    std::unique_ptr<DvmDescriptorPool> build() const;

  private:
    DvmDevice& dvmDevice;
    std::vector<VkDescriptorPoolSize> poolSizes {};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  DvmDescriptorPool(DvmDevice& dvmDevice,
                    uint32_t maxSets,
                    VkDescriptorPoolCreateFlags poolFlags,
                    const std::vector<VkDescriptorPoolSize>& poolSizes);
  ~DvmDescriptorPool();
  DvmDescriptorPool(const DvmDescriptorPool&) = delete;
  DvmDescriptorPool& operator=(const DvmDescriptorPool&) = delete;

  VkDescriptorPool getDescriptorPool() { return descriptorPool; }

  bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                          VkDescriptorSet& descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

  void resetPool();

private:
  DvmDevice& dvmDevice;
  VkDescriptorPool descriptorPool;

  friend class DvmDescriptorWriter;
};

class DvmDescriptorWriter
{
public:
  DvmDescriptorWriter(DvmDescriptorSetLayout& setLayout,
                      DvmDescriptorPool& pool);

  DvmDescriptorWriter& writeBuffer(uint32_t binding,
                                   VkDescriptorBufferInfo* bufferInfo);
  DvmDescriptorWriter& writeImage(uint32_t binding,
                                  VkDescriptorImageInfo* imageInfo);

  bool build(VkDescriptorSet& set);
  void overwrite(VkDescriptorSet& set);

private:
  DvmDescriptorSetLayout& setLayout;
  DvmDescriptorPool& pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace dvm
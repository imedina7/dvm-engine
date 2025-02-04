#pragma once
#include "dvm_device.hpp"

namespace dvm
{
class Texture
{
public:
  Texture(DvmDevice& device, const std::string& filepath);
  ~Texture();

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture(Texture&&) = delete;
  Texture& operator=(Texture&&) = delete;

  VkSampler getSampler() const { return sampler; }
  VkImageView getImageView() const { return imageView; }
  VkImageLayout getImageLayout() const { return imageLayout; }
  VkDescriptorImageInfo getDescriptorImageInfo();

private:
  void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
  void generateMipmaps();

  int width, height, mipLevels;
  DvmDevice& dvmDevice;
  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;
  VkSampler sampler;
  VkFormat imageFormat;
  VkImageLayout imageLayout;
};
}  // namespace dvm
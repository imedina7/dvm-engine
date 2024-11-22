#pragma once
#include <glm/glm.hpp>
namespace dvm
{
enum MaterialType
{
  MtPbr = 0
};
class DvmMaterial
{
  struct PbrMaterial
  {
    glm::vec3 diffuseColor {0.8f};
    glm::vec3 specularColor {0.8f};
    glm::vec3 emissiveColor {0.f};
    float roughness = 0.5f;
    float metallic = 0.f;
  };

public:
  DvmMaterial() = default;
  ~DvmMaterial() = default;

private:
  int materialType;
};
}  // namespace dvm
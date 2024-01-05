#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "dvm_model.hpp"

namespace dvm
{
using id_t = unsigned int;
struct Transform2dComponent
{
  glm::vec2 translation {};
  glm::vec2 scale {1.f, 1.f};
  float rotation;

  glm::mat2 mat2()
  {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 scaleMat {{scale.x, 0.f}, {0.0f, scale.y}};
    glm::mat2 rotationMat {{c, -s}, {s, c}};
    return rotationMat * scaleMat;
  }
};

struct TransformComponent
{
  glm::vec3 translation {};
  glm::vec3 scale {1.f, 1.f, 1.f};
  glm::vec3 rotation {};

  glm::mat4 mat4()
  {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4 {{
                          scale.x * (c1 * c3 + s1 * s2 * s3),
                          scale.x * (c2 * s3),
                          scale.x * (c1 * s2 * s3 - c3 * s1),
                          0.0f,
                      },
                      {
                          scale.y * (c3 * s1 * s2 - c1 * s3),
                          scale.y * (c2 * c3),
                          scale.y * (c1 * c3 * s2 + s1 * s3),
                          0.0f,
                      },
                      {
                          scale.z * (c2 * s1),
                          scale.z * (-s2),
                          scale.z * (c1 * c2),
                          0.0f,
                      },
                      {translation.x, translation.y, translation.z, 1.0f}};
  }
};

class DvmGameObject
{
public:
  static DvmGameObject createGameObject()
  {
    static id_t currentId = 0;
    return DvmGameObject(currentId++);
  }

  DvmGameObject(const DvmGameObject&) = delete;
  DvmGameObject& operator=(const DvmGameObject&) = delete;
  DvmGameObject(DvmGameObject&&) = default;
  DvmGameObject& operator=(DvmGameObject&&) = default;

  std::shared_ptr<DvmModel> model {};
  glm::vec3 color {};
  TransformComponent transform {};

private:
  DvmGameObject(id_t objId)
      : id {objId} {};
  id_t id;
};
}  // namespace dvm
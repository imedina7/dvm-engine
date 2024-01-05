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

  glm::mat4 mat4();
  glm::mat3 normalMatrix();
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
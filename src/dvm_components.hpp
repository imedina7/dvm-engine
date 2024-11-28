#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

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

struct TagComponent
{
  std::string tag;
  TagComponent() = default;
  TagComponent(const TagComponent&) = default;
  TagComponent(const std::string& tagName)
      : tag {tagName} {};
};

struct TransformComponent
{
  glm::vec3 translation {};
  glm::vec3 scale {1.f, 1.f, 1.f};
  glm::vec3 rotation {};

  glm::mat4 mat4();
  glm::mat3 normalMatrix();

  TransformComponent() { translation = glm::vec3(0.f); }

  TransformComponent(const TransformComponent& transform)
  {
    translation = transform.translation;
    scale = transform.scale;
    rotation = transform.rotation;
  }

  ~TransformComponent() = default;

  TransformComponent(const glm::vec3 _translation)
      : translation {_translation}
  {
  }

  TransformComponent(const glm::vec3 _translation, const glm::vec3 _scale)
      : translation {_translation}
      , scale {_scale}
  {
  }

  TransformComponent(const glm::vec3 _translation,
                     const glm::vec3 _scale,
                     const glm::vec3 _rotation)
      : translation {_translation}
      , scale {_scale}
      , rotation {_rotation}
  {
  }

  bool operator==(const TransformComponent& other)
  {
    return other.rotation == this->rotation && other.scale == this->scale
        && other.translation == this->translation;
  }
};

struct PointLightComponent
{
  float lightIntensity = 1.0f;
  glm::vec3 lightColor = glm::vec3(1.0f);
  PointLightComponent(float intensity, glm::vec3 color)
      : lightIntensity {intensity}
      , lightColor {color} {};

  bool operator==(const PointLightComponent& other)
  {
    return other.lightColor == this->lightColor
        && other.lightIntensity == this->lightIntensity;
  }
};
}  // namespace dvm
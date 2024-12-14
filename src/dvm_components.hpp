#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

#include "dvm_model.hpp"

namespace dvm
{
using glm::vec2, glm::vec3, glm::sin, glm::cos, glm::pi;

struct RigidBodyComponent
{
  vec3 velocity {0.f};
  vec3 direction {0.f};

  glm::mat4 transform {1.f};

  RigidBodyComponent() = default;
  RigidBodyComponent(const vec3 velocity, const vec3 direction)
      : velocity {velocity}
      , direction {direction}
  {
  }
  RigidBodyComponent(const glm::mat4& transform)
      : transform {transform}
  {
  }
};

struct PhysicsMaterialComponent
{
  float friction {1.f};
  float weightKg {10.f};
  // absorb collision by default
  float spring {.2f};
};
struct Transform2dComponent
{
  vec2 translation {};
  vec2 scale {1.f, 1.f};
  float rotation;

  glm::mat2 mat2()
  {
    const float s = sin(rotation);
    const float c = cos(rotation);
    glm::mat2 scaleMat {{scale.x, 0.f}, {0.0f, scale.y}};
    glm::mat2 rotationMat {{c, -s}, {s, c}};
    return rotationMat * scaleMat;
  }
};

struct TransformComponent
{
  vec3 translation {};
  vec3 scale {1.f, 1.f, 1.f};
  vec3 rotation {};

  glm::mat4 mat4();
  glm::mat3 normalMatrix();

  TransformComponent() { translation = vec3(0.f); }

  TransformComponent(const TransformComponent& transform)
  {
    translation = transform.translation;
    scale = transform.scale;
    rotation = transform.rotation;
  }

  ~TransformComponent() = default;

  TransformComponent(const vec3 _translation)
      : translation {_translation}
  {
  }

  TransformComponent(const vec3 _translation, const vec3 _scale)
      : translation {_translation}
      , scale {_scale}
  {
  }

  TransformComponent(const vec3 _translation,
                     const vec3 _scale,
                     const vec3 _rotation)
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

struct CollisionObject
{
  std::unique_ptr<DvmModel> model;
  CollisionObject(std::unique_ptr<DvmModel> _model)
      : model {std::move(_model)}
  {
  }
};

struct Renderable
{
  std::unique_ptr<DvmModel> model;
  Renderable(std::unique_ptr<DvmModel> _model)
      : model {std::move(_model)}
  {
  }
};

struct PointLightComponent
{
  float lightIntensity = 1.0f;
  vec3 lightColor = vec3(1.0f);
  PointLightComponent(float intensity, vec3 color)
      : lightIntensity {intensity}
      , lightColor {color} {};

  bool operator==(const PointLightComponent& other)
  {
    return other.lightColor == this->lightColor
        && other.lightIntensity == this->lightIntensity;
  }
};
}  // namespace dvm
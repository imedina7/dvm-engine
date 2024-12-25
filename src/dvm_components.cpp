#include "dvm_components.hpp"

namespace dvm
{
glm::mat4 TransformComponent::mat4()
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
glm::mat3 TransformComponent::normalMatrix()
{
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);

  const glm::vec3 invScale = 1.0f / scale;

  return glm::mat3 {
      {invScale.x * (c1 * c3 + s1 * s2 * s3),
       invScale.x * (c2 * s3),
       invScale.x * (c1 * s2 * s3 - c3 * s1)},
      {invScale.y * (c3 * s1 * s2 - c1 * s3),
       invScale.y * (c2 * c3),
       invScale.y * (c1 * c3 * s2 + s1 * s3)},
      {invScale.z * (c2 * s1), invScale.z * (-s2), invScale.z * (c1 * c2)}};
}

bool CapsuleCollider::intersects(vec3 position)
{
  float halfHeight = height / 2.0f;
  if (position.y < offset.y - (halfHeight + radius)
      || position.y > offset.y + (halfHeight + radius))
    return false;
  vec3 capsuleTop = vec3(offset.x, offset.y - (halfHeight + radius), offset.z);

  vec3 point = position - capsuleTop;

  float angle = glm::acos(glm::dot(capsuleTop, point)
                          / (glm::length(capsuleTop) * glm::length(point)));

  float c = glm::length(vec2(point.x, point.z)) * glm::cos(angle);
  float closestRadius = glm::sin((point.y / radius) * half_pi<float>());
  if (c > closestRadius)
    return false;
  return true;
}

bool CapsuleCollider::intersects(BoxCollider box)
{
  float halfHeight = height / 2.0f;
  if (!box.intersects(vec3(offset.x, offset.y - halfHeight + radius, offset.z))
      && !box.intersects(
          vec3(offset.x, offset.y + halfHeight + radius, offset.z)))
    return false;
  vec3 halfBoxSize = box.size / 2.0f;
  return true;
}
CapsuleCollider::CapsuleCollider(float radius, float height, vec3 offset)
    : radius {radius}
    , height {height}
    , offset {offset}
{
}

bool BoxCollider::intersects(vec3 position)
{
  vec3 halfSize = size / 2.0f;
  if (position.x < offset.x - halfSize.x || position.x > offset.x + halfSize.x)
    return false;
  if (position.y < offset.y - halfSize.y || position.y > offset.y + halfSize.y)
    return false;
  if (position.z < offset.z - halfSize.z || position.z > offset.z + halfSize.z)
    return false;
  return true;
}

}  // namespace dvm
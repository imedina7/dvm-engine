#pragma once
#include "keyboard_movement_controller.hpp"
#include "dvm_components.hpp"
#include "dvm_camera.hpp"

#include <entt.hpp>
#include <string>

namespace dvm
{
  class Entity;

static std::string getFilePathExtension(const std::string& FileName)
{
  if (FileName.find_last_of(".") != std::string::npos)
    return FileName.substr(FileName.find_last_of(".") + 1);
  return "";
}

struct GlobalUbo;
class Scene
{
public:
  Scene();
  ~Scene() = default;
  Entity createEntity(const std::string& label);
  Entity createEntityWithUUID(const std::string& label);

#ifdef GLTF_ENABLE
  void loadFromGltf(const std::string filepath);

  Scene(const std::string filepath) { loadFromGltf(filepath); }
#endif

  void load();
  GlobalUbo update(float frameTime, glm::vec2 mouseDelta, bool controlCamera, float aspectRatio);

  entt::registry& getRegistry() { return registry; };
  DvmCamera& getCamera();

  private:
  void initSceneCamera();

private:
  entt::registry registry;
  FPSMovementController cameraController {};
  entt::entity cameraEntity;

  friend class Entity;
};
}  // namespace dvm

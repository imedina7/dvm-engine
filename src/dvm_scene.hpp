#pragma once
#include "keyboard_movement_controller.hpp"
#include "dvm_components.hpp"
#include "dvm_camera.hpp"
#include "dvm_entity.hpp"

#include <entt.hpp>
#include <string>

namespace dvm
{

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
  DvmCamera& getCamera() { return camera.getComponent<CameraComponent>().camera; };

private:
  entt::registry registry;
  FPSMovementController cameraController {};
  Entity camera;

  friend class Entity;
};
}  // namespace dvm

#pragma once
#include <entt.hpp>
#include <string>
#include "keyboard_movement_controller.hpp"
#include "dvm_components.hpp"
#include "dvm_camera.hpp"

namespace dvm {

static std::string getFilePathExtension(const std::string& FileName) {
    if (FileName.find_last_of(".") != std::string::npos)
    return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}
  class Scene {
    public:
      Scene(FPSMovementController& controller) {
        cameraController = controller;
      }
      ~Scene() = default;

#ifdef GLTF_ENABLE
      void loadFromGltf(const std::string filepath);

      Scene(const std::string filepath){
        loadFromGltf(filepath);
      }
#endif

      void load();
      void update(float deltaTime,
                  glm::vec2 mouseDelta);
      entt::registry& getRegistry() { return registry; };
      DvmCamera& getCamera() { return camera; };

    private:
      entt::registry registry;
      entt::entity viewerEntity;
      FPSMovementController cameraController;
      DvmCamera camera {};
  };
}

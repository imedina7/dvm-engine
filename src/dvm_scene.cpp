#include "dvm_scene.hpp"
#include "dvm_app.hpp"
#include "dvm_entity.hpp"
#include "dvm_components.hpp"

#ifdef GLTF_ENABLE

#  define TINYGLTF_IMPLEMENTATION
#  define TINYGLTF_NO_STB_IMAGE
#  define TINYGLTF_NO_STB_IMAGE_WRITE
#  include <tinygltf/tinygltf.h>

#endif

namespace dvm
{
Scene::Scene() {
  Entity camera = createEntityWithUUID("Camera");
  camera.addComponent<CameraComponent>();
  cameraEntity = camera.getId();
}

Entity Scene::createEntity(const std::string& label)
{
  return createEntityWithUUID(label);
}

Entity Scene::createEntityWithUUID(const std::string& label)
{
  Entity entity = {registry.create(), this};
  entity.addComponent<TransformComponent>();
  entity.addComponent<TagComponent>(label);
  entity.addComponent<IDComponent>();
  return entity;
}

void Scene::initSceneCamera() {
  cameraEntity = registry.create();
  Entity camera = { cameraEntity, this };
  camera.addComponent<TransformComponent>();
  camera.addComponent<IDComponent>();
  camera.addComponent<TagComponent>("Camera");
}

DvmCamera& Scene::getCamera() {
  Entity camera = { cameraEntity, this };
  return camera.getComponent<CameraComponent>().camera;
}

void Scene::load()

{
  Entity camera = { cameraEntity, this };

  camera
      .updateComponent<TransformComponent>(glm::vec3(0.f, -1.f, -3.f));

  auto& cameraComponent = camera.getComponent<CameraComponent>();

  cameraComponent.camera.setViewDirection(glm::vec3(0.f, 10.f, 2.5f),
                                   glm::vec3(0.f, 0.f, 0.f));

  auto box = createEntity("Cornell Box");

  box.updateComponent<TransformComponent>(glm::vec3(0.f, 0.f, 0.f),
                                          glm::vec3(1.f),
                                          glm::vec3(0.f, glm::pi<float>(), 0.f));
      box.addComponent<ModelComponent>(
          DvmModel::createModelFromFile("shading_test_box.obj"));

  std::vector<entt::entity> lights {registry.create(), registry.create(), registry.create(), registry.create(), registry.create(), registry.create(), registry.create(), registry.create()};

  for (int i = 0; i < lights.size(); i++) {
    std::string lightName = "Light";
    registry.emplace<TagComponent>(lights.at(i),
                                   lightName.append(std::to_string(i)));
    glm::vec3 color {};
    ImGui::ColorConvertHSVtoRGB(i / static_cast<float>(lights.size()), 0.6f, 0.8f, color.r, color.g, color.b);

    registry.emplace<TransformComponent>(lights.at(i),
                                         color,
                                         glm::vec3(.1f),
                                         glm::vec3(0.f));

    registry.emplace<PointLightComponent>(lights.at(i), 0.2f, color);
  }
}

GlobalUbo Scene::update(float frameTime, glm::vec2 mouseDelta, bool controlCamera, float aspectRatio)
{
  Entity camera = { cameraEntity, this };

  if (controlCamera) {
    cameraController.moveInPlaneXZ(
        camera, frameTime, mouseDelta, 0.1f);
  }

  auto& cameraComponent = camera.getComponent<CameraComponent>();
  cameraComponent.camera.setPerspectiveProjection(glm::radians(50.f), aspectRatio, .06f, 100.f);
  GlobalUbo ubo {};

  ubo.projection = cameraComponent.camera.getProjection();
  ubo.view = cameraComponent.camera.getView();
  ubo.inverseView = cameraComponent.camera.getInverseView();
  return ubo;
}

#ifdef GLTF_ENABLE
void Scene::loadFromGltf(const std::string& filepath)
{
  tinygltf::Model model;
  tinygltf::TinyGLTF gltf_ctx;
  std::string err;
  std::string warn;
  std::string ext = getFilePathExtension(filepath);

  bool ret = false;

  if (ext.compare("glb") == 0) {
    std::cout << "Reading binary glTF" << std::endl;
    ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, filepath.c_str());
  } else {
    std::cout << "Reading ASCII glTF" << std::endl;
    ret = gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, filepath.c_str());
  }

  for (auto node : model.scenes.at(0).nodes) {
  }

  if (!warn.empty()) {
    printf("Warn: %s\n", warn.c_str());
  }

  if (!err.empty()) {
    printf("Err: %s\n", err.c_str());
  }

  if (!ret) {
    printf("Failed to parse glTF\n");
    return -1;
  }
}
#endif
}  // namespace dvm
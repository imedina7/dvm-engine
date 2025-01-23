#include "dvm_scene.hpp"
#include "dvm_app.hpp"

#ifdef GLTF_ENABLE

#  define TINYGLTF_IMPLEMENTATION
#  define TINYGLTF_NO_STB_IMAGE
#  define TINYGLTF_NO_STB_IMAGE_WRITE
#  include <tinygltf/tinygltf.h>

#endif

namespace dvm
{
Scene::Scene() {camera = createEntityWithUUID("Camera");
camera.addComponent<CameraComponent>();
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

void Scene::load()

{

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

  std::vector<entt::entity> lights {registry.create()};

  for (int i = 0; i < lights.size(); i++) {
    std::string lightName = "Light";
    registry.emplace<TagComponent>(lights.at(i),
                                   lightName.append(std::to_string(i)));
    registry.emplace<TransformComponent>(lights.at(i),
                                         glm::vec3(0.f, 0.f, -2.f),
                                         glm::vec3(.1f),
                                         glm::vec3(0.f));

    registry.emplace<PointLightComponent>(
        lights.at(i), 0.2f, glm::vec3(1.f, 1.f, 1.f));
  }
}

GlobalUbo Scene::update(float frameTime, glm::vec2 mouseDelta, bool controlCamera, float aspectRatio)
{
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
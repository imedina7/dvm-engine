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

void Scene::load()

{
  viewerEntity = registry.create();

  registry.emplace<TransformComponent>(viewerEntity,
                                       glm::vec3(0.f, -1.f, -3.f));

  camera.setViewDirection(glm::vec3(0.f, 10.f, 2.5f), glm::vec3(0.f, 0.f, 0.f));

  auto box = registry.create();

  registry.emplace<TransformComponent>(box,
                                       glm::vec3(0.f, 0.f, 0.f),
                                       glm::vec3(1.f),
                                       glm::vec3(0.f, glm::pi<float>(), 0.f));
  registry.emplace<ModelComponent>(
      box, DvmModel::createModelFromFile("shading_test_box.obj"));

  std::vector<entt::entity> lights {registry.create()};

  for (auto lightEntity : lights) {
    registry.emplace<TransformComponent>(
        lightEntity, glm::vec3(0.f, 0.f, -2.f), glm::vec3(.1f), glm::vec3(0.f));

    registry.emplace<PointLightComponent>(
        lightEntity, 0.2f, glm::vec3(1.f, 1.f, 1.f));
  }
}

void Scene::update(float frameTime, glm::vec2 mouseDelta)
{
  cameraController.moveInPlaneXZ(
      camera, frameTime, registry, viewerEntity, mouseDelta, 0.1f);
  auto& renderer = DvmApp::getInstance().getRenderer();

  float aspect = renderer.getAspectRatio();
  camera.setPerspectiveProjection(glm::radians(50.f), aspect, .06f, 100.f);
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
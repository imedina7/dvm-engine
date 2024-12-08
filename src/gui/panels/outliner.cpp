#include "outliner.hpp"
#include "dvm_app.hpp"
#include "dvm_scene.hpp"
#include <iostream>
#include <entt.hpp>

namespace dvm::gui
{
void Outliner::draw()
{
  DvmApp& app = DvmApp::getInstance();
  Scene& scene = app.getScene();
  entt::registry& registry = scene.getRegistry();

  std::cout << "Drawing " << getTitle() << "...\n";

  ImGui::Begin(getTitle().data());

  ImGui::SetNextWindowBgAlpha(0.7f);

  auto view = registry.view<TagComponent, TransformComponent>();
  for (auto&& [entity, tag, transform] : view.each()) {
    // Use SetNextItemOpen() so set the default state of a node to be open. We
    // could also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag
    // to achieve the same thing!
    // ImGui::SetNextItemOpen(true, ImGuiCond_Once);

    // Here we use PushID() to generate a unique base ID, and then the "" used
    // as TreeNode id won't conflict. An alternative to using 'PushID() +
    // TreeNode("", ...)' to generate a unique ID is to use
    // 'TreeNode((void*)(intptr_t)i, ...)', aka generate a dummy pointer-sized
    // value to be hashed. The demo below uses that technique. Both are fine.

    ImGui::PushID(static_cast<int>(entity));
    if (ImGui::TreeNode("", tag.tag.c_str())) {
      // for (const auto component : view.components(entity)) {
      //   std::string componentName = component.type().name();
      //   ImGui::Text(componentName.data());
      // }
      ImGui::BeginGroup();
      float translation[3] = {transform.translation.x,
                              transform.translation.y,
                              transform.translation.z};
      float rotation[3] = {glm::degrees<float>(transform.rotation.x),
                           glm::degrees<float>(transform.rotation.y),
                           glm::degrees<float>(transform.rotation.z)};
      float scale[3] = {
          transform.scale.x, transform.scale.y, transform.scale.z};

      if (ImGui::DragFloat3(
              "Translation", translation, 0.2f, -99999999.f, 99999999.f))
      {
        transform.translation =
            glm::vec3(translation[0], translation[1], translation[2]);
        registry.replace<TransformComponent>(entity, transform);
      }
      if (ImGui::DragFloat3("Rotation", rotation, 0.2f, -360.f, 360.f)) {
        transform.rotation = glm::vec3(glm::radians<float>(rotation[0]),
                                       glm::radians<float>(rotation[1]),
                                       glm::radians<float>(rotation[2]));
        registry.replace<TransformComponent>(entity, transform);
      }
      if (ImGui::DragFloat3("Scale", scale, 0.2f, -99999999.f, 99999999.f)) {
        transform.scale = glm::vec3(scale[0], scale[1], scale[2]);
        registry.replace<TransformComponent>(entity, transform);
      }
      ImGui::EndGroup();
      ImGui::SameLine();
      if (ImGui::SmallButton("+")) {
      }
      ImGui::TreePop();
    }
    ImGui::PopID();
  }
  ImGui::End();
}
}  // namespace dvm::gui
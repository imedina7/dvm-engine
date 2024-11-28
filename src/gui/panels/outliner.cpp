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

  for (int i = 0; i < 5; i++) {
    // Use SetNextItemOpen() so set the default state of a node to be open. We
    // could also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag
    // to achieve the same thing!
    if (i == 0)
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);

    // Here we use PushID() to generate a unique base ID, and then the "" used
    // as TreeNode id won't conflict. An alternative to using 'PushID() +
    // TreeNode("", ...)' to generate a unique ID is to use
    // 'TreeNode((void*)(intptr_t)i, ...)', aka generate a dummy pointer-sized
    // value to be hashed. The demo below uses that technique. Both are fine.
    ImGui::PushID(i);
    if (ImGui::TreeNode("", "Child %d", i)) {
      ImGui::Text("blah blah");
      ImGui::SameLine();
      if (ImGui::SmallButton("button")) {
      }
      ImGui::TreePop();
    }
    ImGui::PopID();
  }
  ImGui::Text("This is some useful text.");
  state.isVisible = !ImGui::Button("Close", ImVec2(50, 30));
  ImGui::End();
}
bool Outliner::isVisible()
{
  return state.isVisible;
}
}  // namespace dvm::gui
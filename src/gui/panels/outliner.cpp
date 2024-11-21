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

  ImGui::Begin(getTitle().data());  // Create a window called "Hello, world!"
                                    // and append into it.

  ImGui::Text("This is some useful text.");
  state.isVisible = !ImGui::Button("Close", ImVec2(50, 30));
  ImGui::End();
}
bool Outliner::isVisible()
{
  return state.isVisible;
}
}  // namespace dvm::gui
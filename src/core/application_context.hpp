#pragma once

#include "../dvm_frame_info.hpp"
#include <glm/glm.hpp>

namespace dvm
{
// Forward declarations
class DvmDevice;
class DvmWindow;
class Scene;

// Shared context that all layers can access
// This enables clean data flow between layers without tight coupling
struct ApplicationContext
{
  // Core infrastructure (owned by DvmApp)
  DvmDevice* device = nullptr;
  DvmWindow* window = nullptr;

  // Frame data (updated each frame)
  GlobalUbo currentFrameUbo {};
  float deltaTime = 0.0f;
  float aspectRatio = 1.0f;

  // Input state (updated by input callbacks)
  glm::vec2 mouseDelta {0.0f};
  glm::vec2 mousePosition {0.0f};

  // Scene state
  Scene* activeScene = nullptr;

  // Control flags
  bool cameraControlEnabled = true;  // False when UI captures input
  bool isRunning = true;
};

}  // namespace dvm

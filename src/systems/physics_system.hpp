#include "dvm_device.hpp"
#include "dvm_frame_info.hpp"
#include "dvm_pipeline.hpp"

namespace dvm
{
class PhysicsSystem
{
public:
  PhysicsSystem() = default;

  PhysicsSystem(const PhysicsSystem&) = delete;
  PhysicsSystem& operator=(const PhysicsSystem&) = delete;

  void update(FrameInfo& frameInfo, GlobalUbo& ubo);
};
}  // namespace dvm
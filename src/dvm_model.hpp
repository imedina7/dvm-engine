#include "dvm_device.hpp"
#include "dvm_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>

namespace dvm
{
class DvmModel
{
  enum PrimitiveType
  {
    PLANE = 0,
    CUBE,
    SPHERE,
    CIRCLE,
    CYLINDER,
    TORUS,
    TOTAL_PRIMITIVES
  };

public:
  struct Vertex
  {
    glm::vec3 position {};
    glm::vec3 color {};
    glm::vec3 normal {};
    glm::vec2 uv {};

    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();

    bool operator==(const Vertex& other) const
    {
      return position == other.position && color == other.color
          && normal == other.normal && uv == other.uv;
    }
  };

  struct Builder
  {
    std::vector<Vertex> vertices {};
    std::vector<uint32_t> indices {};

    void loadModel(const std::string& filepath);
    void makePrimitive(PrimitiveType primitive,
                       glm::vec3 resolution = glm::vec3(1),
                       glm::vec3 position = glm::vec3(0.f),
                       glm::vec3 scale = glm::vec3(1.f),
                       glm::vec3 rotation = glm::vec3(0.f));
  };

  DvmModel(DvmDevice& device, const Builder& builder);
  ~DvmModel();
  DvmModel(const DvmModel&) = delete;
  DvmModel& operator=(const DvmModel&) = delete;

  static std::unique_ptr<DvmModel> createModelFromFile(
      DvmDevice& device, const std::string& filepath);

  static std::unique_ptr<DvmModel> createSphere(
      DvmDevice& device,
      int segments,
      int rings,
      float radius,
      glm::vec3 position = glm::vec3(0.f),
      glm::vec3 scale = glm::vec3(1.f),
      glm::vec3 rotation = glm::vec3(0.f));

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex>& vertices);
  void createIndexBuffers(const std::vector<uint32_t>& indices);

  DvmDevice& dvmDevice;

  std::unique_ptr<DvmBuffer> vertexBuffer;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<DvmBuffer> indexBuffer;
  uint32_t indexCount;
};
}  // namespace dvm
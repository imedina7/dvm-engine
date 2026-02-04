#pragma once
#include "dvm_device.hpp"
#include "dvm_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <tinyobjloader.h>

#ifndef ENGINE_DIR
#  define ENGINE_DIR "../"
#endif

namespace dvm
{
class DvmModel
{
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
    std::vector<tinyobj::material_t> materials {};

    void loadModel(const std::string& filepath);
  };

  DvmModel(DvmDevice& device, const Builder& builder);
  ~DvmModel();
  DvmModel(const DvmModel&) = delete;
  DvmModel& operator=(const DvmModel&) = delete;

  static std::unique_ptr<DvmModel> createModelFromFile(
      const std::string& filepath);

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

  static const std::string getModelsPath()
  {
    std::string modelsPath = "assets/models/";
    return ENGINE_DIR + modelsPath;
  }

private:
  void createVertexBuffers(const std::vector<Vertex>& vertices);
  void createIndexBuffers(const std::vector<uint32_t>& indices);
  void createMaterials(const std::vector<tinyobj::material_t>& materials);

  DvmDevice& dvmDevice;

  std::unique_ptr<DvmBuffer> vertexBuffer;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<DvmBuffer> indexBuffer;
  uint32_t indexCount;
};
struct ModelComponent
{
  std::unique_ptr<DvmModel> model;
  ModelComponent(std::unique_ptr<DvmModel> _model)
      : model {std::move(_model)}
  {
  }
};
}  // namespace dvm
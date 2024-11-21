#include "dvm_model.hpp"
#include "dvm_utils.hpp"

#include <cassert>
#include <cstring>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std
{
template<>
struct hash<dvm::DvmModel::Vertex>
{
  size_t operator()(dvm::DvmModel::Vertex const& vertex) const
  {
    size_t seed = 0;
    dvm::hashCombine(
        seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};
}  // namespace std

namespace dvm
{
DvmModel::DvmModel(DvmDevice& device, const DvmModel::Builder& builder)
    : dvmDevice {device}
{
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
  createMaterials(builder.materials);
}

DvmModel::~DvmModel() {}

void DvmModel::createMaterials(const std::vector<tinyobj::material_t>& materials){

}

void DvmModel::createVertexBuffers(const std::vector<Vertex>& vertices)
{
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3.");

  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

  uint32_t vertexSize = sizeof(vertices[0]);

  DvmBuffer stagingBuffer {
      dvmDevice,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
          | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();

  stagingBuffer.writeToBuffer((void*)vertices.data());

  vertexBuffer = std::make_unique<DvmBuffer>(
      dvmDevice,
      vertexSize,
      vertexCount,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  dvmDevice.copyBuffer(
      stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void DvmModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
  indexCount = static_cast<uint32_t>(indices.size());

  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  uint32_t indexSize = sizeof(indices[0]);

  DvmBuffer stagingBuffer {
      dvmDevice,
      indexSize,
      indexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
          | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();

  stagingBuffer.writeToBuffer((void*)indices.data());

  indexBuffer = std::make_unique<DvmBuffer>(
      dvmDevice,
      indexSize,
      indexCount,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  dvmDevice.copyBuffer(
      stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

std::unique_ptr<DvmModel> DvmModel::createModelFromFile(
    DvmDevice& device, const std::string& filepath)
{
  Builder builder {};
  builder.loadModel(ENGINE_DIR + filepath);
  std::cout << "Vertex count: " << builder.vertices.size() << std::endl;
  std::cout << "Materials count: " << builder.materials.size() << std::endl;
  return std::make_unique<DvmModel>(device, builder);
}

void DvmModel::bind(VkCommandBuffer commandBuffer)
{
  VkBuffer buffers[] = {vertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(
        commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}

void DvmModel::draw(VkCommandBuffer commandBuffer)
{
  if (!hasIndexBuffer) {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    return;
  }
  vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}

std::vector<VkVertexInputBindingDescription>
DvmModel::Vertex::getBindingDescriptions()
{
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription>
DvmModel::Vertex::getAttributeDescriptions()
{
  std::vector<VkVertexInputAttributeDescription> attrDescriptions {};

  attrDescriptions.push_back(
      {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
  attrDescriptions.push_back(
      {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
  attrDescriptions.push_back(
      {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
  attrDescriptions.push_back(
      {3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

  return attrDescriptions;
}

void DvmModel::Builder::loadModel(const std::string& filepath)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::string warn, err;

  std::cout << "Loading object in path \"" << filepath << "\"\n";

  size_t indx = filepath.find_last_of('/');
  std::string dir = filepath.substr(0, indx);

  std::cout << "materials loaded from: " << dir << "\n";

  if (!tinyobj::LoadObj(
          &attrib, &shapes, &materials, &warn, &err, filepath.c_str(), dir.c_str()))
  {
    throw std::runtime_error(warn + err);
  }

  if(materials.size() > 0) {
    for(const auto& material: materials) {
      std::cout << "Loaded material: \"" << material.name << "\"\n";
    }
  } else {
    std::cout << "No materials loaded" << "\n";
  }

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices {};
  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex {};
      if (index.vertex_index >= 0) {
        vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                           attrib.vertices[3 * index.vertex_index + 1],
                           attrib.vertices[3 * index.vertex_index + 2]};

        vertex.color = {attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]};
      }
      if (index.normal_index >= 0) {
        vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                         attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2]};
      }
      if (index.texcoord_index >= 0) {
        vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                     attrib.texcoords[2 * index.texcoord_index + 1]};
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

}  // namespace dvm
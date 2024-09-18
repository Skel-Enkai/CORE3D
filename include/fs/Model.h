#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <glm/fwd.hpp>
#include <json/json.h>
#include <string>
#include <vector>

#include "fs/Mesh.h"
#include "fs/Shader.h"
#include "fs/Texture.h"

using json = nlohmann::json;

class Model
{
public:
  Model(std::string file,
        glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0),
        glm::vec3 translation = glm::vec3(1.0, 1.0, 1.0),
        glm::quat rotation = glm::quat(1.0, 0.0, 0.0, 0.0),
        unsigned int instancing = 1,
        std::vector<glm::mat4> instanceMatrix = {},
        std::vector<glm::mat4> rotationMatrices = {});

  Model(std::string file,
        unsigned int instancing,
        std::vector<glm::mat4> instanceMatrix,
        std::vector<glm::mat4> rotationMatrices);

  void Draw(Shader &shader, Camera &camera);
  void Draw(Shader &shader, Shader &secondaryShader, unsigned int mirrorTexture, Camera &camera);
  void DrawShadow(Shader &shader);

  glm::vec3 position = glm::vec3(1.0, 1.0, 1.0);
  glm::quat rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
  glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);

protected:
  void SetTextures(Shader &shader);

  std::vector<Mesh> meshes;
  std::vector<glm::vec3> translationsMeshes;
  std::vector<glm::quat> rotationsMeshes;
  std::vector<glm::vec3> scalesMeshes;
  std::vector<glm::mat4> matricesMeshes;
  std::vector<glm::mat4> instanceMatrices;
  std::vector<glm::mat4> rotationMatrices;

private:
  std::string file;
  std::vector<unsigned char> data;
  json JSON;

  unsigned int instancing;

  std::vector<Texture> textures;

  void loadMesh(unsigned int indMesh, std::string name);
  void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

  std::vector<unsigned char> getData();
  std::vector<float> getFloats(json accessor);
  std::vector<GLuint> getIndices(json accessor);
  std::vector<Texture> getTextures();

  std::vector<Vertex> assembleVertices(std::vector<glm::vec3> positions,
                                       std::vector<glm::vec3> normals,
                                       std::vector<glm::vec2> texUVs);

  std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
  std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
  std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};

#endif

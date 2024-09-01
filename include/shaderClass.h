#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <string>

std::string get_file_contents(const char *filename);

class Shader
{
public:
  Shader(std::string vertexFile);
  Shader(std::string vertexFile, std::string fragmentFile);
  Shader(std::string vertexFile, std::string geometryFile, std::string fragmentFile);

  void setFloat(const std::string &uniformName, const float floatToSet);
  void setInt(const std::string &uniformName, const int intToSet);
  void setVec3(const std::string &uniformName, const glm::vec3 &vector);
  void setVec4(const std::string &uniformName, const glm::vec4 &vector);
  void setMat4(const std::string &uniformName, const glm::mat4 &matrix);

  void Activate();
  void Delete();

private:
  // Reference ID of the Shader Program
  GLuint ID;
  void logErrors(unsigned int shader, const char *type, std::string fileName = "");
};

#endif

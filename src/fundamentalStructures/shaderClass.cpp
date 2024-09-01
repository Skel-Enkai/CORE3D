#include "shaderClass.h"

#include <cerrno>
#include <fstream>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return (contents);
  }
  throw(errno);
}

Shader::Shader(std::string vertexFile)
{
  std::string vertexCode = get_file_contents(vertexFile.c_str());
  const char *vertexSource = vertexCode.c_str();

  // Create Vertex Shader Object and get its reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach Vertex Shader source to the Vertex Shader Object
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertexShader);
  logErrors(vertexShader, "VERTEX", vertexFile);

  // Create Shader Program Object and get its reference
  ID = glCreateProgram();
  // Attach the Vertex and Fragment Shaders to the Shader Program
  glAttachShader(ID, vertexShader);
  // Wrap-up/Link all the shaders together into the Shader Program
  glLinkProgram(ID);
  logErrors(ID, "PROGRAM");

  // Delete the now useless Vertex and Shader objects
  glDeleteShader(vertexShader);
}

Shader::Shader(std::string vertexFile, std::string fragmentFile)
{
  std::string vertexCode = get_file_contents(vertexFile.c_str());
  std::string fragmentCode = get_file_contents(fragmentFile.c_str());

  const char *vertexSource = vertexCode.c_str();
  const char *fragmentSource = fragmentCode.c_str();

  // Create Vertex Shader Object and get its reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach Vertex Shader source to the Vertex Shader Object
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertexShader);
  logErrors(vertexShader, "VERTEX", vertexFile);

  // Create Fragment Shader Object and get its reference
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  // Attach Fragment Shader source to the Fragment Shader Object
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  // Compile the Fragment Shader into machine code
  glCompileShader(fragmentShader);
  logErrors(fragmentShader, "FRAGMENT", fragmentFile);

  // Create Shader Program Object and get its reference
  ID = glCreateProgram();
  // Attach the Vertex and Fragment Shaders to the Shader Program
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  // Wrap-up/Link all the shaders together into the Shader Program
  glLinkProgram(ID);
  logErrors(ID, "PROGRAM");

  // Delete the now useless Vertex and Shader objects
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::Shader(std::string vertexFile, std::string geometryFile, std::string fragmentFile)
{
  std::string vertexCode = get_file_contents(vertexFile.c_str());
  std::string geometryCode = get_file_contents(geometryFile.c_str());
  std::string fragmentCode = get_file_contents(fragmentFile.c_str());

  const char *vertexSource = vertexCode.c_str();
  const char *geometrySource = geometryCode.c_str();
  const char *fragmentSource = fragmentCode.c_str();

  // Create Vertex Shader Object and get its reference
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Attach Vertex Shader source to the Vertex Shader Object
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  // Compile the Vertex Shader into machine code
  glCompileShader(vertexShader);
  logErrors(vertexShader, "VERTEX", vertexFile);

  // Compile Geometry Shader
  GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometryShader, 1, &geometrySource, NULL);
  glCompileShader(geometryShader);
  logErrors(geometryShader, "GEOMETRY", geometryFile);

  // Create Fragment Shader Object and get its reference
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  // Attach Fragment Shader source to the Fragment Shader Object
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  // Compile the Fragment Shader into machine code
  glCompileShader(fragmentShader);
  logErrors(fragmentShader, "FRAGMENT", fragmentFile);

  // Create Shader Program Object and get its reference
  ID = glCreateProgram();
  // Attach the Vertex and Fragment Shaders to the Shader Program
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, geometryShader);
  glAttachShader(ID, fragmentShader);
  // Wrap-up/Link all the shaders together into the Shader Program
  glLinkProgram(ID);
  logErrors(ID, "PROGRAM");

  // Delete the now useless Vertex and Shader objects
  glDeleteShader(vertexShader);
  glDeleteShader(geometryShader);
  glDeleteShader(fragmentShader);
}

void Shader::Activate() { glUseProgram(ID); }

void Shader::Delete() { glDeleteShader(ID); }

void Shader::logErrors(unsigned int object, const char *type, std::string fileName)
{
  GLint hasSucceeded;
  char infoLog[1024];
  if (strcmp(type, "PROGRAM") != 0)
  {
    glGetShaderiv(object, GL_COMPILE_STATUS, &hasSucceeded);
    if (hasSucceeded == GL_FALSE)
    {
      glGetShaderInfoLog(object, 1024, NULL, infoLog);
      std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n"
                << "in file: " << fileName << "\n"
                << infoLog << std::endl;
    }
  }
  else
  {
    glGetProgramiv(object, GL_LINK_STATUS, &hasSucceeded);
    if (hasSucceeded == GL_FALSE)
    {
      glGetProgramInfoLog(object, 1024, NULL, infoLog);
      std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << infoLog << std::endl;
    }
  }
}

void Shader::setFloat(const std::string &uniformName, const float floatToSet)
{
  glUniform1f(glGetUniformLocation(ID, uniformName.c_str()), floatToSet);
}

void Shader::setInt(const std::string &uniformName, const int intToSet)
{
  glUniform1i(glGetUniformLocation(ID, uniformName.c_str()), intToSet);
}

void Shader::setVec3(const std::string &uniformName, const glm::vec3 &vector)
{
  glUniform3fv(glGetUniformLocation(ID, uniformName.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setVec4(const std::string &uniformName, const glm::vec4 &vector)
{
  glUniform4fv(glGetUniformLocation(ID, uniformName.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setMat4(const std::string &uniformName, const glm::mat4 &matrix)
{
  glUniformMatrix4fv(glGetUniformLocation(ID, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

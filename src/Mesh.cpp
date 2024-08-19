#include "Mesh.h"

#include <string>
#include"EBO.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::string name)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
  Mesh::name = name;

	mVAO.Bind();

	VBO mVBO(vertices);
	EBO mEBO(indices);
	// Links VBO attributes to the mVAO
	mVAO.LinkAttrib(mVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	mVAO.LinkAttrib(mVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	mVAO.LinkAttrib(mVBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	mVAO.LinkAttrib(mVBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	// Unbinds to prevent accidental modification 
	mVAO.Unbind();
	mVBO.Unbind();
	mEBO.Unbind();
}

void Mesh::Draw
(
 	Shader& shader, 
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
)
{
	mVAO.Bind();
  
	// Take care of the Camera Matrix
  shader.setVec3("camPos", camera.Position);
	camera.setCamMatrix(shader, "camMatrix");
	
	// forms matrices
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rot = glm::mat4_cast(rotation);
	glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

	// sends matrices to the Shader
  shader.setMat4("translation", trans);
  shader.setMat4("rotation", rot);
  shader.setMat4("scale", sca);
  shader.setMat4("initMatrix", matrix);

	// Draw the actual Mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::Draw
(
 	Shader& primaryShader,
  Shader& secondaryShader,
  unsigned int mirrorTexture,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
)
{
	mVAO.Bind();
  // forms matrices
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rot = glm::mat4_cast(rotation);
	glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

  if (Mesh::name == "mirror")
  {
    secondaryShader.Activate();

    glActiveTexture(GL_TEXTURE0 + 99);
    glBindTexture(GL_TEXTURE_2D, mirrorTexture);	
  	// Take care of the Camera Matrix
    secondaryShader.setVec3("camPos", camera.Position);
  	camera.setCamMatrix(secondaryShader, "camMatrix");
    // sends matrices to the secondaryShader
    secondaryShader.setMat4("translation", trans);
    secondaryShader.setMat4("rotation", rot);
    secondaryShader.setMat4("scale", sca);
    secondaryShader.setMat4("initMatrix", matrix);
  }

  else 
  {
	  primaryShader.Activate();

    // Take care of the Camera Matrix
    primaryShader.setVec3("camPos", camera.Position);
  	camera.setCamMatrix(primaryShader, "camMatrix");
    // sends matrices to the primaryShader
    primaryShader.setMat4("translation", trans);
    primaryShader.setMat4("rotation", rot);
    primaryShader.setMat4("scale", sca);
    primaryShader.setMat4("initMatrix", matrix);
  }

  // Draw the actual Mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

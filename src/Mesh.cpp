#include "Mesh.h"

#include <string>
#include"EBO.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, std::string name)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
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
	shader.Activate();
	mVAO.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse") 
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}
	// Take care of the Camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");
	
	// forms matrices
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rot = glm::mat4_cast(rotation);
	glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

	// sends matrices to the Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "initMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mirrorTexture);	
  	// Take care of the Camera Matrix
  	glUniform3f(glGetUniformLocation(secondaryShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
  	camera.Matrix(secondaryShader, "camMatrix");
     // sends matrices to the secondaryShader
  	glUniformMatrix4fv(glGetUniformLocation(secondaryShader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
  	glUniformMatrix4fv(glGetUniformLocation(secondaryShader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
  	glUniformMatrix4fv(glGetUniformLocation(secondaryShader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
  	glUniformMatrix4fv(glGetUniformLocation(secondaryShader.ID, "initMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
  }

  else 
  {
	  primaryShader.Activate();

    unsigned int numDiffuse = 0;
	  unsigned int numSpecular = 0;

	  for (unsigned int i = 0; i < textures.size(); i++)
	  {
		  std::string num;
  		std::string type = textures[i].type;
  		if (type == "diffuse") 
  		{
  			num = std::to_string(numDiffuse++);
  		}
  		else if (type == "specular")
  		{
  			num = std::to_string(numSpecular++);
  		}
  		textures[i].texUnit(primaryShader, (type + num).c_str(), i);
  		textures[i].Bind();
  	}
    // Take care of the Camera Matrix
  	glUniform3f(glGetUniformLocation(primaryShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
  	camera.Matrix(primaryShader, "camMatrix");
    // sends matrices to the primaryShader
  	glUniformMatrix4fv(glGetUniformLocation(primaryShader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
  	glUniformMatrix4fv(glGetUniformLocation(primaryShader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
  	glUniformMatrix4fv(glGetUniformLocation(primaryShader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
  	glUniformMatrix4fv(glGetUniformLocation(primaryShader.ID, "initMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
  }

  // Draw the actual Mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"Mesh.h"

const unsigned int width = 800;
const unsigned int height = 800;

int mac_width, mac_height;

const float scale = 2.0f;

// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f * scale)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f * scale, 1.0f * scale)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f * scale, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

int main() 
{	
	// Initialise GLFW
	glfwInit();
	
	// Tell GLFW what version of OpenGL were are running
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW we are using the CORE profile
	// Core profile only contains modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// Create a GLFWwindow object of width by height pixels, naming it "YoutubeOpenGL"
	GLFWwindow * window = glfwCreateWindow(width, height, "OpenGLEngine", NULL, NULL);

	// Error check if the window fails to create
	if (window == NULL)
	{	// pipes error message to cout, and Terminates GLFW
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1; 
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	
	// Load GLAD so it configures OpenGL
	gladLoadGL();
	
	// Mac Screen Correction
	glfwGetFramebufferSize(window, &mac_width, &mac_height);

	// Specify the viewport of OpenGL in the Window 
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, mac_width, mac_height);
	
	std::string texPath = "resources/textures/";
	Texture textures[]
	{
		Texture((texPath + "planks.png").c_str(), "diffuse", 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture((texPath + "planksSpec.png").c_str(), "specular", 1, GL_RED, GL_RED, GL_UNSIGNED_BYTE)
	};

	std::string shaderPath = "resources/shaders/";
	Shader shaderProgram((shaderPath + "default.vert").c_str(), (shaderPath + "default.frag").c_str());
	// Converts Arrays to Vectors of variable length
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	
	Mesh floor(verts, ind, tex);

	Shader lightShader((shaderPath + "light.vert").c_str(), (shaderPath + "light.frag").c_str());
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	std::vector <Texture> lightTex;

	Mesh light(lightVerts, lightInd, lightTex);
	
	glm::vec4 lightColor = glm::vec4(1.0f, 0.8f, 0.9f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 1.5f, 0.3f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	//lightModel = glm::rotate(lightModel, (3.14/4.0)f, glm::vec3(0.0f, 1.0f, 0.5f)); 

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);
	pyramidModel = glm::scale(pyramidModel, scale * glm::vec3(1.0f, 1.0f, 1.0f));

	glm::mat4 pyramidModelRotation = glm::mat4(1.0f);
	pyramidModelRotation = glm::rotate(pyramidModelRotation, 0.4f, glm::vec3(1.0f, 0.0f, 0.0f));

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(pyramidModelRotation));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Stops behind layers from displaying during animation (Tests for depth continiously)
	glEnable(GL_DEPTH_TEST);

	// Initialises Camera Object with resolution and position 
	Camera camera(width, height, glm::vec3(0.0f, 0.5f, 2.0f));

	// Main while loop 
	while(!glfwWindowShouldClose(window))
 	{
		// Specify the colour of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new colour to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Gets Input to move the camera
		camera.Inputs(window);
		// Calls Matrix function of Camera object to set the Camera view in the shaderProgram
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		floor.Draw(shaderProgram, camera);
		light.Draw(lightShader, camera);

		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	shaderProgram.Delete();
	lightShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);	
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

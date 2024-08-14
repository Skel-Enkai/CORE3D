#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 currentPos;
out vec3 Normal;
out vec2 texCoord;

// The final view matrix formed by the Camera 
uniform mat4 camMatrix;
// Model matrices
uniform mat4 initMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{	
	currentPos = vec3(initMatrix * translation * scale * rotation * vec4(aPos, 1.0));

	Normal = vec3(rotation * vec4(aNormal, 1.0));

	texCoord = aTex;

	// Outputs the positions/coordinates of all vertices with respective to the camMatrix
	gl_Position = camMatrix * vec4(currentPos, 1.0);
}

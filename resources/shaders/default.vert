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
uniform mat4 completeMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

// translation in the negative y-direction by -1,
// then flip on the x-axis (only works for values within the specified y-range)
// I hand computed this and feel very proud :D
const mat3 coordinateTransformMatrix = mat3(vec3(1.0, 0.0, 0.0), 
					    vec3(0.0, -1.0, 0.0),
					    vec3(0.0, 1.0, 1.0));

void main()
{	
	currentPos = vec3(completeMatrix * vec4(aPos, 1.0));

	//Normal = aNormal;
	Normal = vec3(rotation * vec4(aNormal, 1.0));

	// Solved tutorial issue, by translating the format from gltf standard to glm, by using the coordinatesTransformMatrix
	texCoord =  vec2(coordinateTransformMatrix * vec3(aTex, 1.0));
	 
	// Outputs the positions/coordinates of all vertices with respective to the camMatrix
	gl_Position = camMatrix * vec4(currentPos, 1.0);
}

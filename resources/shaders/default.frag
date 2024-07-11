#version 330 core

// Outputs the colors in RGBA
out vec4 FragColor;

in vec3 currentPos;
in vec3 Normal;
in vec2 texCoord;

// The texture unit uniforms
uniform sampler2D diffuse0;
uniform sampler2D specular0;
// Color from the light cube
uniform vec4 lightColor;

uniform vec3 lightPos;
uniform vec3 camPos;

// Inverse Square Law (Point Light)
vec4 pointLight() 
{
	vec3 lightVec = currentPos - lightPos;
	float dist = length(lightVec);
	float scale = 1.5;
	float a = 0.5;
	float b = 0.2;

	// modified formula to allow for scaling of a and b by 1/k to give same graph stretched
	float intensity = 1.0f / (pow(scale * a * dist, 2) + scale * b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.20f;
	
	// diffuse lighting
	vec3 normal = normalize(Normal);
	// Uses the lightPos and the current Vertex position to calculate the angle/vector
	// at which light hits the surface
	vec3 lightDirection = normalize(lightVec);
	// Uses dot product to calculate how close in angle the two vectors are
	float diffuse = max(dot(normal, -lightDirection), 0.0f);

	// Specular lighting
	float specularLight = 0.25f;
	vec3 viewDirection = normalize(camPos -currentPos);
	vec3 reflectionDirection = reflect(lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;
	
	//Display distance zones
//	if (dist > 2.0f) {
//	return vec4(0.0f, 1.0f, 0.0f, 1.0f);
//	}
//	else if (dist > 1.0f) {
//	return vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	}

	// Outputs the final calcualted color
	return texture(diffuse0, texCoord) * lightColor * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity;
}


vec4 directLight()
{
	// ambient lighting
	float ambient = 0.10f;
	
	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(0.0f, -1.0f, 0.0f));

	// Uses dot product to calculate how close in angle the two vectors are
	float diffuse = max(dot(normal, -lightDirection), 0.0f);

	// Specular lighting
	float specularLight = 0.25f;
	vec3 viewDirection = normalize(camPos -currentPos);
	vec3 reflectionDirection = reflect(lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// Outputs the final calcualted color
	return texture(diffuse0, texCoord) * lightColor * (diffuse + ambient) + texture(specular0, texCoord).r * specular;
	
}

vec4 spotLight() 
{
	// cosine values of the angle 
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;
	
	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(currentPos - lightPos));

	// Uses dot product to calculate how close in angle the two vectors are
	float diffuse = max(dot(normal, -lightDirection), 0.0f);

	// Specular lighting
	float specularLight = 0.25f;
	vec3 viewDirection = normalize(camPos -currentPos);
	vec3 reflectionDirection = reflect(lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	float angle = dot(vec3(0.0f, -1.0f, 0.0f), lightDirection);
	float intensity = clamp((outerCone - angle) / (outerCone - innerCone), 0.0f, 1.0f);

	// Outputs the final calcualted color
	return texture(diffuse0, texCoord) * lightColor * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity;
}


void main()
{
	FragColor = directLight();
}

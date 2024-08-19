#version 330 core

// Outputs the colors in RGBA
out vec4 FragColor;

in vec3 Position;
in vec3 Colour;
in vec3 Normal;
in vec2 TexCoord;

// The texture unit uniforms
uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;


vec4 directLight()
{
	// ambient lighting
	float ambient = 0.40f;
	
	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(-0.2f, -0.4f, 0.2f));

	// Uses dot product to calculate how close in angle the two vectors are.
  // Use Absolute value for grass, as light can shine through grass.
	float diffuse = max(abs(dot(normal, -lightDirection)), 0.0f);

	// Specular lighting
	float specularLight = 0.2f;
	vec3 viewDirection = normalize(camPos -Position);
	vec3 reflectionDirection = reflect(lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

  // If Alpha Value is very low, don't even bother drawing it!
  if (texture(diffuse0, TexCoord).a < 0.1)
    discard;

  //if (normal.y > 0.5)
  //  return vec4(0.0, 1.0, 0.0, 1.0);
  //if (normal.x > 0.5)
  //  return vec4(1.0, 0.0, 0.0, 1.0);
  //if (normal.z > 0.5)
  //  return vec4(0.0, 0.0, 1.0, 1.0);

	// Outputs the final calcualted color
  return (texture(diffuse0, TexCoord) * (diffuse + ambient) + texture(specular0, TexCoord).r * specular) * lightColor;
	
}

float near = 0.1f;
float far = 100.0f;

// linearize the F-Depth formula: F_depth= (1/z- 1/near)/(1/far - 1/near);
float linearizeDepth(float depth)
{
  return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) * (far - near));
}

// steepness: controls how quickly the depth value varies
// offset: controls which set value of distance depth is halfway done, (which distance represents the depth value 0.5f)
float logisticDepth(float depth)
{
  float steepness = 0.5f;
  float offset = 0.5f;
  float zVal = linearizeDepth(depth);
  return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
	FragColor = directLight();
}

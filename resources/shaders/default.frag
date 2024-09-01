#version 330 core

// Outputs the colors in RGBA
out vec4 FragColour;

in vec4 FragPosLight;
in vec3 Position;
in vec3 Normal;
in vec3 Colour;
in vec2 TexCoord;

// The texture unit uniforms
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;
// Color from the light cube
uniform vec4 lightColor;

uniform vec3 lightPos;
uniform vec3 camPos;

float specularLight = 0.45f;

// Inverse Square Law (Point Light)
vec4 pointLight() 
{
	vec3 lightVec = Position - lightPos;
	float dist = length(lightVec);
	float scale = 0.1;
	float a = 0.5;
	float b = 0.2;

	// modified formula to allow for scaling of a and b by 1/k to give same graph stretched
	float intensity = 1.0f / (pow(scale * a * dist, 2) + scale * b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.35f;
	
	// diffuse lighting
	vec3 normal = normalize(Normal);
	// Uses the lightPos and the current Vertex position to calculate the angle/vector
	// at which light hits the surface
	vec3 lightDirection = normalize(lightVec);
	// Uses dot product to calculate how close in angle the two vectors are
	float diffuse = max(dot(normal, -lightDirection), 0.0f);

	// Specular lighting
  float specular = 0.0f;
  if (diffuse != 0.0f)
  {
    vec3 viewDirection = normalize(Position - camPos);
	  // Blinn-Phong Halfway Vector
    vec3 halfwayVec = -normalize(viewDirection + lightDirection);
	  float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 32);
	  specular = specAmount * specularLight;
  }
	
	// Display distance zones
//	if (dist > 2.0f) {
//	return vec4(0.0f, 1.0f, 0.0f, 1.0f);
//	}
//	else if (dist > 1.0f) {
//	return vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	}

	// Outputs the final calcualted color
	return texture(diffuse0, TexCoord) * lightColor * (diffuse * intensity + ambient) + texture(specular0, TexCoord).r * specular * intensity;
}


vec4 directLight()
{
	// ambient lighting
	float ambient = 0.20f;
	
	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(0.0f, -1.0f, 0.0f));

	// Uses dot product to calculate how close in angle the two vectors are
	float diffuse = max(dot(normal, -lightDirection), 0.0f);

	// Specular lighting
  float specular = 0.0f;
  if (diffuse != 0.0f)
  {
    vec3 viewDirection = normalize(Position - camPos);
	  // Blinn-Phong Halfway Vector
    vec3 halfwayVec = -normalize(viewDirection + lightDirection);
	  float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 32);
	  specular = specAmount * specularLight;
  }

  // 1.0f means no shadow and 0.0f means shadow at it's greatest.
  float shadow = 0.0f;
  // Get lightCoords in clipped space
  vec3 lightCoords = FragPosLight.xyz / FragPosLight.w;
  if (lightCoords.z <= 1.0f)
  {
    lightCoords = (lightCoords + 1.0f) / 2.0f;
    float currentDepth = lightCoords.z;
    float bias = max(0.025f * (1.0f - dot(Normal, -lightDirection)), 0.0005f);
    
    int sampleRadius = 2;
    vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);

    for (int y = -sampleRadius; y <= sampleRadius; y++)
    {
      for (int x = -sampleRadius; x <= sampleRadius; x++)
      {
        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
        if (currentDepth > closestDepth + bias)
          shadow += 1.0f;
      }
    }
    shadow /= pow((sampleRadius * 2 + 1), 2);
    shadow = 1.0f - shadow;
  }

//  if (normal.y > 0.8)
//    return vec4(0.0, 1.0, 0.0, 1.0);
//  if (normal.x > 0.8)
//    return vec4(1.0, 0.0, 0.0, 1.0);
//  if (normal.z > 0.8)
//    return vec4(0.0, 0.0, 1.0, 1.0);

	// Outputs the final calcualted color
  return (texture(diffuse0, TexCoord) * (diffuse * shadow + ambient) + texture(specular0, TexCoord).r * specular * shadow) * lightColor;
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
	vec3 lightDirection = normalize(vec3(Position - lightPos));

	// Uses dot product to calculate how close in angle the two vectors are
	float diffuse = max(dot(normal, -lightDirection), 0.0f);

	// Specular lighting
  float specular = 0.0f;
  if (diffuse != 0.0f)
  {
    vec3 viewDirection = normalize(Position - camPos);
	  // Blinn-Phong Halfway Vector
    vec3 halfwayVec = -normalize(viewDirection + lightDirection);
	  float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 32);
	  specular = specAmount * specularLight;
  }

	float angle = dot(vec3(0.0f, -1.0f, 0.0f), lightDirection);
	float intensity = clamp((outerCone - angle) / (outerCone - innerCone), 0.0f, 1.0f);

	// Outputs the final calcualted color
	return (texture(diffuse0, TexCoord) * (diffuse * intensity + ambient) + texture(specular0, TexCoord).r * specular * intensity) * lightColor;
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
	FragColour = directLight();

  // Basic Fog Lighting
  //float depth = logisticDepth(gl_FragCoord.z);
  //FragColor = directLight() * (1.0f - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);

  // FUNKY SHIT: THE COMMA DOIN SOMETHIN
  //FragColor = vec4(directLight().x/2, 0.0f, 0.0f, 1.0f) * (1.0f, - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}

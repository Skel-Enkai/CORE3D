#version 330 core

// Outputs the colors in RGBA
out vec4 FragColor;

in vec4 FragPosLight;
in vec3 Position;
in vec3 Colour;
in vec3 Normal;
in vec2 TexCoord;

// The texture unit uniforms
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;

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
  float specular = 0.0f;
  if (diffuse != 0.0f)
  {
	  float specularLight = 0.45f;
    vec3 viewDirection = normalize(Position - camPos);
	  // Blinn-Phong Halfway Vector
    vec3 halfwayVec = -normalize(viewDirection + lightDirection);
	  float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 32);
	  specular = specAmount * specularLight;
  }

  // If Alpha Value is very low, don't even bother drawing it!
  if (texture(diffuse0, TexCoord).a < 0.1)
    discard;

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
        if (currentDepth >= closestDepth + bias)
          shadow += 1.0f;
      }
    }
    shadow /= pow((sampleRadius * 2 + 1), 2);
    shadow = 1.0f - shadow;
  }

  //if (normal.y > 0.5)
  //  return vec4(0.0, 1.0, 0.0, 1.0);
  //if (normal.x > 0.5)
  //  return vec4(1.0, 0.0, 0.0, 1.0);
  //if (normal.z > 0.5)
  //  return vec4(0.0, 0.0, 1.0, 1.0);

	// Outputs the final calcualted color
  return (texture(diffuse0, TexCoord) * (diffuse * shadow + ambient) + texture(specular0, TexCoord).r * specular * shadow) * lightColor;
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

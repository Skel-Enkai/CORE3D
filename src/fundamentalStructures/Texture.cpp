#include"Texture.h"
#include<iostream>

Texture::Texture(const char* image, std::string texType, GLint slot)
{
	type = texType;
	int widthImg, heightImg, numColCh;
	/*stbi_set_flip_vertically_on_load(true);*/
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
	if (bytes == NULL) {
		std::cout << "Failed to find image." << std::endl;
	}

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// for GL_TEXTURE_WRAP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, 1.0f);
	
	// for: GL_CLAMP_TO_BORDER
	// glTexParameteri(GL_TEXTURE_2D, GL_CLAMP_TO_BORDER, GL_LINEAR);
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f}
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

  // returns texture is SRGB space
  if (type == "diffuse")
  {
    if (numColCh == 4)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    else if (numColCh == 3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
    else if (numColCh == 1)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
    else
      throw std::invalid_argument("Automatic Texture type recognition failed.");
  }
  // returns texture in linear space
  else 
  {
    if (numColCh == 4)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    else if (numColCh == 3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
    else if (numColCh == 1)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
    else
      throw std::invalid_argument("Automatic Texture type recognition failed.");
  }


	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform)
{
  shader.setInt(uniform, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}

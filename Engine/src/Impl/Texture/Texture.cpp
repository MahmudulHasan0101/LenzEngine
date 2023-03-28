#include"Texture.h"

namespace lenz
{
	Texture::Texture(uint32_t textureType)
		: m_Type(textureType)
	{
	}

	void Texture::Delete()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::UploadToShader(Shader& shader, const std::string& name)
	{
		shader.UploadUniformUnsignedInt(name, m_RendererID);
	}

	void Texture::BindToSlot(uint8_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(m_Type, m_RendererID);
		glActiveTexture(0);
	}
//_____________________________________________________________________________________________________________________________________________________________________
//																		T E X T U R E   2 D											 
//_____________________________________________________________________________________________________________________________________________________________________

	Texture2D::Texture2D()
		: Texture(TextureType::TEXTURE2D)
	{
	}

	Texture2D::Texture2D(const std::string& path, uint8_t slot)
		: Texture(TextureType::TEXTURE2D)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				dataFormat = GL_RGB;
			}

			m_DataFormat = dataFormat;

			//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glGenTextures(1, &m_RendererID);
			//glActiveTexture(0);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
							
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
			// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
			// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
			
			//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);

			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glActiveTexture(0);
		}
		else LZ_CORE_ERROR("No texture found at path: " + path);
	}
}
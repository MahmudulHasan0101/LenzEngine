#include"Texture.h"

namespace lenz
{
	Texture::Texture()
	{
	}


	Texture::Texture(uint32_t textureType)
		: m_Type(textureType)
	{
	}

	void Texture::Delete()
	{
		if (m_RendererID != static_cast<uint32_t>(-1) && m_RendererID != 0) {
			glDeleteTextures(1, &m_RendererID);
			m_RendererID = static_cast<uint32_t>(-1);
		}
	}

	void Texture::UploadToShader(Shader& shader, const std::string& name)
	{
		shader.UploadUniformInt(name, m_Slot);
	}

	void Texture::BindToSlot(uint8_t slot)
	{
		m_Slot = slot;
		glActiveTexture(GL_TEXTURE0 + m_Slot);
		glBindTexture(m_Type, m_RendererID);
		//glActiveTexture(0);
	}
//_____________________________________________________________________________________________________________________________________________________________________
//																		T E X T U R E   2 D											 
//_____________________________________________________________________________________________________________________________________________________________________

	Texture2D::Texture2D()
		: Texture(TextureType::TEXTURE2D)
	{
	}

	Texture2D::Texture2D(const char* path, uint8_t slot)
		: Texture(TextureType::TEXTURE2D)
	{
		Init(path, slot);
	}


	Texture2D::Texture2D(const void* data, int width, int height, int channels, int dataformat, int datatype, uint8_t slot)
		:Texture(TextureType::TEXTURE2D)
	{
		Init(data, width, height, channels, dataformat, datatype, slot);
	}

	Texture2D::Texture2D(const std::string& path, uint8_t slot)
		:Texture(TextureType::TEXTURE2D)
	{
		Init(path.c_str(), slot);
	}

	Texture2D::Texture2D(const void* data, int width, int height, Texture2DType type, int slot)
		:Texture(TextureType::TEXTURE2D)
	{
		Init(data, width, height, type, slot);
	}


	void Texture2D::Init(const void* data, int width, int height, int channels, int dataformat, int datatype, uint8_t slot)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, dataformat, datatype, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glActiveTexture(0);
	}


	void Texture2D::Init(const std::string& path, uint8_t slot)
	{
		Init(path.c_str(), slot);
	}

	void Texture2D::Init(const char* path, uint8_t slot)
	{
		int width, height, channels;
		//stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path, &width, &height, &channels, 0);
		}

		if (data)
		{
			GLenum internalFormat = 0, dataFormat = 0;

			switch (channels)
			{
			case 4:
				dataFormat = GL_RGBA;
				break;
			case 3:
				dataFormat = GL_RGB;
				break;
			case 2:
				dataFormat = GL_RG;
				break;
			case 1:
				dataFormat = GL_RED;
				break;

			default:
				break;
			}

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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);

			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);
			//glActiveTexture(0);
		}
		else LZ_CORE_ERROR("No texture found at path: " + std::string(path));
	}

	void Texture2D::Init(const void* data, int width, int height, Texture2DType type, int slot)
	{
		unsigned int internalType = (unsigned int)type >> 32;
		unsigned int dataFormate = ((unsigned int)type >> 16) << 16;
		unsigned int datatype = (unsigned int)type << 32;

		

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, internalType, width, height, 0, dataFormate, datatype, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glActiveTexture(0);
	}
}
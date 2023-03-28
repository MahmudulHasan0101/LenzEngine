#pragma once
#include"stb_image.h"
#include"Lenz/Core.h"
#include"Lenz/Renderer/Shader.h"

namespace lenz
{
	class Texture
	{
	public:
		Texture(uint32_t textureType);
		void Delete();

		inline int GetType() const { return m_Type; }
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline int GetDateFormate() const { return m_DataFormat; }
		inline int GetChannels() const { return m_Channels; }
		inline int GetRendererID() const { return m_RendererID;  }

		void UploadToShader(Shader& shader, const std::string& name);
		void BindToSlot(uint8_t slot);
	protected:
		uint32_t m_RendererID;

		int	m_Width, m_Height,
			m_Channels,
			m_DataFormat;

		bool m_IsLoaded = false;
		uint32_t m_Type;

		
	};


	class Texture2D : public Texture
	{
	public:
		Texture2D();
		Texture2D(const std::string& path, uint8_t slot);
	};
}
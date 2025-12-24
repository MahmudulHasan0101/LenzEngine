#pragma once
#include"stb_image.h"
#include"Lenz/Core.h"
#include"Lenz/Renderer/Shader.h"

namespace lenz
{

	enum class Texture2DType
	{
		// 1-channel float formats
		_1Float32 = (((GL_R32F << 16) | GL_RED) << 16) | GL_FLOAT,
		_1Float16 = (((GL_R16F << 16) | GL_RED) << 16) | GL_HALF_FLOAT,

		// 2-channel float formats
		_2Float32 = (((GL_RG32F << 16) | GL_RG) << 16) | GL_FLOAT,
		_2Float16 = (((GL_RG16F << 16) | GL_RG) << 16) | GL_HALF_FLOAT,

		// 3-channel float formats
		_3Float32 = (((GL_RGB32F << 16) | GL_RGB) << 16) | GL_FLOAT,
		_3Float16 = (((GL_RGB16F << 16) | GL_RGB) << 16) | GL_HALF_FLOAT,

		// 4-channel float formats
		_4Float32 = (((GL_RGBA32F << 16) | GL_RGBA) << 16) | GL_FLOAT,
		_4Float16 = (((GL_RGBA16F << 16) | GL_RGBA) << 16) | GL_HALF_FLOAT,

		// 1-channel unsigned byte
		_1UInt8 = (((GL_R8 << 16) | GL_RED) << 16) | GL_UNSIGNED_BYTE,

		// 2-channel unsigned byte
		_2UInt8 = (((GL_RG8 << 16) | GL_RG) << 16) | GL_UNSIGNED_BYTE,

		// 3-channel unsigned byte
		_3UInt8 = (((GL_RGB8 << 16) | GL_RGB) << 16) | GL_UNSIGNED_BYTE,

		// 4-channel unsigned byte
		_4UInt8 = (((GL_RGBA8 << 16) | GL_RGBA) << 16) | GL_UNSIGNED_BYTE
	};
	


	class Texture
	{
	public:
		Texture();
		Texture(uint32_t textureType);
		void Delete();

		inline int GetType() const { return m_Type; }
		inline int GetRendererID() const { return m_RendererID;  }
		inline uint8_t GetSlot() const { return m_Slot; }

		void UploadToShader(Shader& shader, const std::string& name);
		void BindToSlot(uint8_t slot);
	protected:
		uint32_t m_RendererID{-1u};

		uint32_t m_Type;
		uint8_t  m_Slot;
	};



	class Texture2D : public Texture
	{
	public:
		Texture2D();
		Texture2D(const char* path, uint8_t slot);
		Texture2D(const void* data, int width, int height, int channels, int dataformat, int datatype, uint8_t slot);
		Texture2D(const std::string& path, uint8_t slot);
		Texture2D(const void* data, int width, int height, Texture2DType type, int slot);

		void Init(const char* path, uint8_t slot);
		void Init(const void* binarydata, int width, int height, int channels, int dataformat, int datatype, uint8_t slot);
		void Init(const std::string& path, uint8_t slot);
		void Init(const void* data, int width, int height, Texture2DType type, int slot);

	};
}
#pragma once
#include"Lenz/Core.h"

#include <memory>

namespace lenz
{
	struct TextureType
	{
		const static uint32_t TEXTURE1D = GL_TEXTURE_1D;
		const static uint32_t TEXTURE2D = GL_TEXTURE_2D;
		const static uint32_t TEXTUREBUFFER = GL_TEXTURE_BUFFER;
		const static uint32_t CUBEMAP = GL_TEXTURE_CUBE_MAP;
	};

	struct DepthTestTypes
	{
		const static uint32_t
			LESS = GL_LESS,
			LEQUAL = GL_LEQUAL,
			NEVER = GL_NEVER,
			EQUEL = GL_EQUAL,
			GREATER = GL_GREATER,
			GEQUAL = GL_GEQUAL,
			NOTEQUAL = GL_NOTEQUAL;
	};

	struct BufferFluidity
	{
		const static uint32_t
			Static = GL_STATIC_DRAW,
			Dynamic = GL_DYNAMIC_DRAW;
	};

	typedef BufferFluidity UniformFluidity;

	struct BufferType
	{
		const static uint32_t
			None = 0,
			VertexBuffer = GL_ARRAY_BUFFER,
			IndexBuffer = GL_ELEMENT_ARRAY_BUFFER,
			InstanceBuffer = GL_ARRAY_BUFFER,
			IndirectBuffer = GL_DRAW_INDIRECT_BUFFER,
			ShaderStorage = GL_SHADER_STORAGE_BUFFER;
	};

	struct UniformType
	{
		const static uint32_t
			None = 0,
			Float = 1,
			Int = 2,
			Mat4 = 3,
			Mat3 = 4,
			Mat2 = 5,
			IntArray = 6,
			Float2 = 7,
			Float3 = 8,
			Float4 = 9;
	};

}

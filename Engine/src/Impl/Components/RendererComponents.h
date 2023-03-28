#pragma once
#include"Lenz.h"


#define LZ_COMPONENT_DATA_TYPE(type) inline static unsigned int GetFrameworkType() { return lenz::GetOpenGLType<type>(); }\
									 inline static unsigned int GetTypeSize() { return sizeof (type); }

namespace RenderComponent
{
	// Components:
	struct Position
	{
		glm::vec3 position{ 1.0f };

		Position(float x, float y, float z) : position(x, y, z) {}
		Position() = default;
		Position(const Position&) = default;
		Position(const glm::vec3& position)
			: position(position) {}

		operator glm::vec3& () { return position; }
		operator const glm::vec3& () const { return position; }

		LZ_COMPONENT_DATA_TYPE(float)
	};

	struct Color
	{
		glm::vec4 color{ 1.0f };

		Color(float r, float g, float b, float a) : color(r, g, b, a) {}
		Color() = default;
		Color(const Color&) = default;
		Color(const glm::vec4& color)
			: color(color) {}

		operator glm::vec4& () { return color; }
		operator const glm::vec4& () const { return color; }

		LZ_COMPONENT_DATA_TYPE(float)
	};

	struct TexturePosition
	{
		glm::vec2 position{ 1.0f };

		TexturePosition(float x, float y) : position(x, y) {}
		TexturePosition() = default;
		TexturePosition(const TexturePosition&) = default;
		TexturePosition(const glm::vec2& position)
			: position(position) {}

		operator glm::vec2& () { return position; }
		operator const glm::vec2& () const { return position; }

		LZ_COMPONENT_DATA_TYPE(float)
	};

	struct TextureSlot
	{
		int slot;

		TextureSlot(int slot) : slot(slot) { }
		TextureSlot() = default;
		TextureSlot(const TextureSlot&) = default;
		
		operator int& () { return slot; }
		operator const int& () const { return slot; }

		LZ_COMPONENT_DATA_TYPE(float)
	};
}
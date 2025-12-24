#pragma once
#include"Lenz/Core.h"
#include"Lenz/Renderer/Renderer.h"
#include"Lenz/Inputs.h"
#include"glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define LZ_RENDERER_COMPONENT_DATA_TYPE(type, subdivide)\
									inline static constexpr unsigned int GetFrameworkType() { return lenz::OpenGLType<type>; }\
									inline static constexpr unsigned int GetTypeSize() { return sizeof (type); }\
									inline static constexpr unsigned int GetSubDivideNumber() { return subdivide; }\


#pragma region RendererComponents

namespace RendererComponent
{
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

		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)
	};

	struct Normal
	{
		glm::vec3 normal{ 1.0f };

		Normal(float x, float y, float z) : normal(x, y, z) {}
		Normal() = default;
		Normal(const Normal&) = default;
		Normal(const glm::vec3& normal)
			: normal(normal) {}

		operator glm::vec3& () { return normal; }
		operator const glm::vec3& () const { return normal; }

		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)
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

		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)
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

		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)
	};

	struct TextureSlot
	{
		int slot;

		TextureSlot(int slot) : slot(slot) { }
		TextureSlot() = default;
		TextureSlot(const TextureSlot&) = default;

		operator int& () { return slot; }
		operator const int& () const { return slot; }

		LZ_RENDERER_COMPONENT_DATA_TYPE(int, 1)
	};

	struct Transformation 
	{
		Transformation(glm::mat4 mat) : matrix(mat) {}

		Transformation() = default;
		Transformation(const Transformation&) = default;
		Transformation(Transformation&&) = default;
		Transformation& operator=(const Transformation&) = default;
		Transformation& operator=(Transformation&&) = default;
		~Transformation() = default;

		void Scale(float x, float y, float z) {
			matrix = glm::scale(matrix, glm::vec3(x, y, z));
		}
		void ScaleRatio(float ratio) {
			Scale(ratio, ratio, ratio);
		}
		void ScaleX(float x) {
			matrix = glm::scale(matrix, glm::vec3(x, glm::length(glm::vec3(matrix[1])), glm::length(glm::vec3(matrix[2]))));
		}
		void ScaleY(float y) {
			matrix = glm::scale(matrix, glm::vec3(glm::length(glm::vec3(matrix[0])), y, glm::length(glm::vec3(matrix[2]))));
		}
		void ScaleZ(float z) {
			matrix = glm::scale(matrix, glm::vec3(glm::length(glm::vec3(matrix[0])), glm::length(glm::vec3(matrix[1])), z));
		}

		void RotateX(float angle) {
			matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		void RotateY(float angle) {
			matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		void RotateZ(float angle) {
			matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		void MoveX(float x) {
			matrix = glm::translate(matrix, glm::vec3(x, 0.0f, 0.0f));
		}
		void MoveY(float y) {
			matrix = glm::translate(matrix, glm::vec3(0.0f, y, 0.0f));
		}
		void MoveZ(float z) {
			matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, z));
		}

		void Reset() {
			matrix = glm::mat4(1.0f);
		}

		operator glm::mat4& () { return matrix; }
		operator const glm::mat4& () const { return matrix; }
		inline operator const void* () const { return (void*)glm::value_ptr(matrix); }
		inline operator void* () const { return (void*)glm::value_ptr(matrix); }

		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 4)

		glm::mat4 matrix;
	};



	struct Translation {

		Translation() = default;
		Translation(const glm::vec3& pos) : position(pos) {}
		Translation(float x, float y, float z) : position(x, y, z) {}

		void Set(float x, float y, float z) {
			position = glm::vec3(x, y, z);
		}

		void Move(float dx, float dy, float dz) {
			position += glm::vec3(dx, dy, dz);
		}


		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)

		glm::vec3 position{ 0.0f };
	};



	struct Orientation {

		Orientation() = default;
		Orientation(const glm::vec3& angles) : eulerAngles(angles) {}
		Orientation(float pitch, float yaw, float roll)
			: eulerAngles(pitch, yaw, roll) {}

		void Set(float pitch, float yaw, float roll) {
			eulerAngles = glm::vec3(pitch, yaw, roll);
		}

		void Rotate(float dpitch, float dyaw, float droll) {
			eulerAngles += glm::vec3(dpitch, dyaw, droll);
		}

		glm::quat ToQuaternion() const {
			return glm::quat(glm::radians(eulerAngles));
		}

		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)

		glm::vec3 eulerAngles{ 0.0f };
	};


	struct Scale1d{
		Scale1d() = default;
		Scale1d(const glm::vec3& s) : scale(s) {}
		Scale1d(float x, float y, float z) : scale(x, y, z) {}

		void Set(float x, float y, float z) {
			scale = glm::vec3(x, y, z);
		}


		LZ_RENDERER_COMPONENT_DATA_TYPE(float, 1)

		glm::vec3 scale{ 1.0f };

	};


}
#pragma endregion







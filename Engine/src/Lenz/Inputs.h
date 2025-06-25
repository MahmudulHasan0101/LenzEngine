#pragma once
#include"GLFW/glfw3.h"
#include"glm/glm.hpp"


namespace lenz {

	template<typename T>
	inline constexpr uint32_t OpenGLType = 0; // default fallback

	template<> inline constexpr uint32_t OpenGLType<int> = GL_INT;
	template<> inline constexpr uint32_t OpenGLType<char> = GL_BYTE;
	template<> inline constexpr uint32_t OpenGLType<float> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<double> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<unsigned int> = GL_UNSIGNED_INT;
	template<> inline constexpr uint32_t OpenGLType<unsigned char> = GL_UNSIGNED_BYTE;
	template<> inline constexpr uint32_t OpenGLType<unsigned short> = GL_UNSIGNED_SHORT;
	template<> inline constexpr uint32_t OpenGLType<short> = GL_SHORT;

	template<> inline constexpr uint32_t OpenGLType<glm::vec2> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::vec3> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::vec4> = GL_FLOAT;

	template<> inline constexpr uint32_t OpenGLType<glm::ivec2> = GL_INT;
	template<> inline constexpr uint32_t OpenGLType<glm::ivec3> = GL_INT;
	template<> inline constexpr uint32_t OpenGLType<glm::ivec4> = GL_INT;

	template<> inline constexpr uint32_t OpenGLType<glm::uvec2> = GL_UNSIGNED_INT;
	template<> inline constexpr uint32_t OpenGLType<glm::uvec3> = GL_UNSIGNED_INT;
	template<> inline constexpr uint32_t OpenGLType<glm::uvec4> = GL_UNSIGNED_INT;

	template<> inline constexpr uint32_t OpenGLType<glm::dvec2> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dvec3> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dvec4> = GL_DOUBLE;

	// Float matrices
	template<> inline constexpr uint32_t OpenGLType<glm::mat2> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat3> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat4> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat2x3> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat3x2> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat2x4> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat4x2> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat3x4> = GL_FLOAT;
	template<> inline constexpr uint32_t OpenGLType<glm::mat4x3> = GL_FLOAT;

	// Double matrices
	template<> inline constexpr uint32_t OpenGLType<glm::dmat2> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat3> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat4> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat2x3> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat3x2> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat2x4> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat4x2> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat3x4> = GL_DOUBLE;
	template<> inline constexpr uint32_t OpenGLType<glm::dmat4x3> = GL_DOUBLE;

}



/*
namespace lenz
{
	template<typename T>
	inline static uint32_t GetOpenGLType() { return 0; }

	template<>
	inline static uint32_t GetOpenGLType<int>() { return GL_INT; }

	template<>
	inline static uint32_t GetOpenGLType<char>() { return GL_BYTE; }

	template<>
	inline static uint32_t GetOpenGLType<float>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<double>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<unsigned int>() { return GL_UNSIGNED_INT; }

	template<>
	inline static uint32_t GetOpenGLType<unsigned char>() { return GL_UNSIGNED_BYTE; }

	template<>
	inline static uint32_t GetOpenGLType<unsigned short>() { return GL_UNSIGNED_SHORT; }

	template<>
	inline static uint32_t GetOpenGLType<short>() { return GL_SHORT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::vec2>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::vec3>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::vec4>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::ivec2>() { return GL_INT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::ivec3>() { return GL_INT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::ivec4>() { return GL_INT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::uvec2>() { return GL_UNSIGNED_INT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::uvec3>() { return GL_UNSIGNED_INT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::uvec4>() { return GL_UNSIGNED_INT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dvec2>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dvec3>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dvec4>() { return GL_DOUBLE; }

	// GLM float matrices
	template<>
	inline static uint32_t GetOpenGLType<glm::mat2>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat3>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat4>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat2x3>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat3x2>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat2x4>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat4x2>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat3x4>() { return GL_FLOAT; }

	template<>
	inline static uint32_t GetOpenGLType<glm::mat4x3>() { return GL_FLOAT; }

	// GLM double matrices
	template<>
	inline static uint32_t GetOpenGLType<glm::dmat2>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat3>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat4>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat2x3>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat3x2>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat2x4>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat4x2>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat3x4>() { return GL_DOUBLE; }

	template<>
	inline static uint32_t GetOpenGLType<glm::dmat4x3>() { return GL_DOUBLE; }

}
*/
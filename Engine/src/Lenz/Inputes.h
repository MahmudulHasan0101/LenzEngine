#pragma once
#include"GLFW/glfw3.h"

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


}
#pragma once
#include <Lenz/Core.h>
#include "Lenz/Window.h"

#include <unordered_map>
#include <string>

#define USING_GLEW

#include"Buffer.h"
#include"Framebuffer.h"
#include"Lenz/Renderer/Buffer.h"
#include"Shader.h"
#include"Lenz/Utils/Library.h"
#include"Lenz/Utils/Vector.h"


namespace lenz
{
	class App;


	class RendererCommands
	{
	public:
		static void Init();
		static GLFWwindow* CreateNewWindow(const WindowData& data);

		static void SetEvents(Window&);
		static void SetWindowUserPointer(Window&);
		static void SetWindowHint(GLuint, GLuint);
		static void SetViewPort(uint32_t, uint32_t, uint32_t, uint32_t);
		static void EnableDepthTest();
		static void DisableDepthTest();
		static void DepthTestType(uint32_t);

		static void MakeCurrentContext(GLFWwindow*);
		static void DestroyContext(GLFWwindow*);
		static void SwapBuffers(GLFWwindow*);

		static void SetVSync(bool);
		static void PollEvents();
		//void Draw(const DrawBuffer& data);
		static bool ShouldClose(GLFWwindow*);
		static void Clear();

	public:
		static bool VSync;
		static bool isContextCreated;
		static bool isIntellized;

	};




	class Renderer
	{
	public:
		
		static void Draw(uint32_t indecisCount, const ArrayBuffer& varr, const Shader& shader)
		{
			shader.Bind();
			varr.Bind();
			glDrawElements(GL_TRIANGLES, indecisCount, GL_UNSIGNED_INT, 0);
		}


		static void Draw(uint32_t instanceCount, uint32_t indecisCount, const ArrayBuffer& varr, const Shader& shader)
		{
			shader.Bind();
			varr.Bind();
			glDrawElementsInstanced(GL_TRIANGLES, indecisCount, GL_UNSIGNED_INT, 0, instanceCount);
		}

	};

};


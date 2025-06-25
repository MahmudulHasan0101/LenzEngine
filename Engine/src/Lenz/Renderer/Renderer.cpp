#include "Renderer.h"
#include "Lenz/Event/Event.h"
#include "Lenz/Event/ApplicationEvent.h"
#include "Lenz/Event/KeyEvent.h"
#include "Lenz/Event/MouseEvent.h"

#include <Lenz/Core.h>
#include <Lenz/App.h>

void GLAPIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	std::cerr << "[OpenGL ERROR] " << message << std::endl;

	if (severity == GL_DEBUG_SEVERITY_HIGH) {  
		__debugbreak();  
	}
}

void EnableOpenGLDebug() {
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

namespace lenz
{
	bool RendererCommands::isContextCreated = false;
	bool RendererCommands::isIntellized = false;
	bool RendererCommands::VSync = true;

	//Buffer Renderer::Tshape::VertexBuffer = Buffer(BufferType::VertexBuffer);
	//Buffer Renderer::Tshape::IndexBuffer = Buffer(BufferType::IndexBuffer);
	//Buffer Renderer::InstanceBuffer = Buffer(BufferType::VertexBuffer);
	//BufferLayout Renderer::layout;
	//ArrayBuffer Renderer::varray;



	//void RenderSpec::Init(uint32_t vbSize, uint32_t ibSize)
	//{
	//	m_VBO.Init(nullptr, vbSize);
	//	m_IBO.Init(nullptr, ibSize);
	//	m_VAO.Init();
	//}
	//
	//void RenderSpec::SetVertexBufferData(void* data, uint32_t size, uint32_t offset)
	//{
	//	m_VBO.SetData(offset, size, data);
	//}
	//
	//void RenderSpec::SetIndexBufferData(uint32_t* data, uint32_t size, uint32_t offset )
	//{
	//	m_IBO.SetData(offset, size, data);
	//}
	//
	//void RenderSpec::SetShaderPtr(Shader* shader)
	//{
	//	m_Shader = shader;
	//}
	//
	//void RenderSpec::AddVBElement(const BufferElement& vbe)
	//{
	//	m_VBL.AddElement(vbe);
	//}
	//
	//void RenderSpec::Bind()
	//{
	//	m_VAO.AddBuffer(m_VBO, m_VBL);
	//}




	static void ErrorCallback(int error, const char* descirption)
	{
		LZ_CORE_ERROR(descirption);
	}


	void RendererCommands::Init()
	{
		if (!isIntellized)
		{
			isIntellized = true;
			LZ_CORE_ASSERT(glfwInit(), "Could not intellize glfw!");

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
			//glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
			LZ_DEBUG_ONLY(
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
			)
			LZ_CORE_INFO("GLFW intellized!");
			return;
		}
		LZ_CORE_WARN("Cannot intellize glfw again!");
	}

	GLFWwindow* RendererCommands::CreateNewWindow(const WindowData& data)
	{
		GLFWwindow* window = glfwCreateWindow(data.Width, data.Height, data.Title.c_str(), nullptr, nullptr);
		return window;
	}

	void RendererCommands::SetWindowUserPointer(Window& window)
	{
		glfwSetWindowUserPointer(window.GetNativeWindow(), (void*)&window.GetData());
	}

	void RendererCommands::SetEvents(Window& window)
	{
		GLFWwindow* windowPtr = window.GetNativeWindow();
		LZ_CORE_ASSERT(windowPtr, "Window handle is Null!");

		if (&window.GetData() != glfwGetWindowUserPointer(window.GetNativeWindow()))
			LZ_CORE_WARN("User pointer is located to a different Window's data, so events may not run properly.");

		glfwSetWindowSizeCallback(windowPtr, [](GLFWwindow* window, int width, int height)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				data.Width = width;
				data.Height = height;

				WindowResizeEvent e(width, height);
				data.EventCallback(e);
				glfwSetWindowSize(window, width, height);
				glViewport(0, 0, width, height);
			});

		glfwSetWindowCloseCallback(windowPtr, [](GLFWwindow* window)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				WindowCloseEvent e;
				data.EventCallback(e);
			});

		glfwSetKeyCallback(windowPtr, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
					return;
				}

				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallback(e);
					return;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
					return;
				}
				default:
					break;
				}
			});

		glfwSetCharCallback(windowPtr, [](GLFWwindow* window, unsigned int character)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				KeyTypedEvent e(character);
				data.EventCallback(e);
			});

		glfwSetMouseButtonCallback(windowPtr, [](GLFWwindow* window, int button, int action, int mods)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
					return;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
					return;
				}
				}
			});

		glfwSetScrollCallback(windowPtr, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				MouseScrolledEvent e(xoffset, yoffset);
				data.EventCallback(e);
			});

		glfwSetCursorPosCallback(windowPtr, [](GLFWwindow* window, double xpos, double ypos)
			{
				void* ptr = glfwGetWindowUserPointer(window);
				WindowData& data = *(WindowData*)ptr;
				MouseMovedEvent e(xpos, ypos);
				data.EventCallback(e);
			});

		glfwSetErrorCallback(ErrorCallback);

	}

	void RendererCommands::MakeCurrentContext(GLFWwindow* windowPtr)
	{
		LZ_DEBUG_ONLY(glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);)
		glfwMakeContextCurrent(windowPtr);
		glewInit();

		LZ_DEBUG_ONLY(
		EnableOpenGLDebug();
		)
	}

	void RendererCommands::SetWindowHint(GLuint hint, GLuint val)
	{
		glfwWindowHint(hint, val);
	}

	void RendererCommands::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}


	void RendererCommands::SwapBuffers(GLFWwindow* windowPtr)
	{
		glfwSwapBuffers(windowPtr);
	}

	void RendererCommands::DestroyContext(GLFWwindow* windowPtr)
	{
		glfwDestroyWindow(windowPtr);
	}

	void RendererCommands::SetVSync(bool enable)
	{
		if (enable) glfwSwapInterval(1);
		else glfwSwapInterval(0);
	}

	void RendererCommands::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void RendererCommands::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void RendererCommands::DepthTestType(uint32_t t)
	{
		glDepthFunc(t);
	}


	void RendererCommands::PollEvents()
	{
		glfwPollEvents();
	}
	 
	void RendererCommands::Clear()
	{
		glClearColor(0.25f, 0.20f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	bool RendererCommands::ShouldClose(GLFWwindow* windowPtr)
	{
		return glfwWindowShouldClose(windowPtr);
	}

	//void RendererDraw(const RenderSpec& scene, const Shader& shader)
	//{
	//	shader.Bind();
	//	scene.GetVertexArray().Bind();
	//	glDrawElements(GL_TRIANGLES, scene.GetIndexBuffer().GetSize()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
	//	scene.GetVertexArray().Unbind();
	//	shader.Unbind();
	//}
	//
	//void RendererCommands::Draw(const RenderSpec& scene, const Shader& shader, const Framebuffer& framebuffer)
	//{
	//	framebuffer.Bind();
	//	Draw(scene, shader);
	//	framebuffer.Unbind();
	//}
	//
	//void RendererCommands::Draw(const RenderSpec& scene, const Shader& shader, const Camera& camera)
	//{
	//	shader.Bind();
	//	shader.SetMat4(camera.GetName(), camera);
	//	scene.GetVertexArray().Bind();
	//	glDrawElements(GL_TRIANGLES, scene.GetIndexBuffer().GetSize() / sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
	//	scene.GetVertexArray().Unbind();
	//	shader.Unbind();
	//}
	//
	//void RendererCommands::Draw(const RenderSpec& scene, const Shader& shader, const Camera& camera, const Framebuffer& framebuffer)
	//{
	//	framebuffer.Bind();
	//	Draw(scene, shader, camera);
	//	framebuffer.Unbind();
	//}

}

#include "Window.h"
#include "Lenz/Renderer/Renderer.h"
#include "Core.h"

namespace lenz
{
	Window::Window()
	{
	}

	Window::~Window()
	{
		//Shutdown();
	}

	void Window::OnUpdate(TimeStep& ts)
	{
		RendererCommands::PollEvents();
		RendererCommands::SwapBuffers(m_Window);

		if (RendererCommands::ShouldClose(m_Window)) Shutdown();
	}

	void Window::SetVSync(bool enable)
	{
		RendererCommands::SetVSync(enable);
	}

	bool Window::IsVSync() const
	{
		return RendererCommands::VSync;
	}


	void Window::Init(const WindowData& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		WindowData a = props;

		m_Window = RendererCommands::CreateNewWindow(props);
		LZ_CORE_ASSERT((m_Window != nullptr), "GUI Window is NULL!");
		LZ_CORE_INFO("GUI window created!");

		RendererCommands::MakeCurrentContext(m_Window);
		LZ_INFO("Current context settled!");

		RendererCommands::SetViewPort(0, 0, m_Data.Width, m_Data.Height);
		SetVSync(RendererCommands::VSync);
	}

	void Window::Shutdown()
	{
		if (m_Window != nullptr) RendererCommands::DestroyContext(m_Window);
	}


}
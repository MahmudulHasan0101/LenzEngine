#include "App.h"
#include "Renderer/Renderer.h"


namespace lenz
{
	App* App::s_Instance = nullptr;

	Window* App::s_Window = nullptr;
	TimeStep App::m_TimeStep;

	App::App() {
		if (s_Instance == nullptr)
		{
			App::s_Instance = this;
			LZ_CORE_INFO("App Created!");

			RendererCommands::Init();

			s_Window = GetAppWindow();
			s_Window->Init();

			s_Window->SetEventCallback(LZ_BIND_EVENT_FN(App::OnEvent));
			RendererCommands::SetWindowUserPointer(*s_Window);
			RendererCommands::SetEvents(*s_Window);
		}
		else LZ_CORE_WARN("App already created!");
	}
	App::~App()
	{
		delete(s_Window);
	}

	void App::Run()
	{
		while (m_Running)
		{
			m_TimeStep.Tick();
			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(m_TimeStep);
				}
			}
			s_Window->OnUpdate(m_TimeStep);
		}
	}


	void App::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(LZ_BIND_EVENT_FN(App::OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(LZ_BIND_EVENT_FN(App::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
		}
	}


	bool App::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}

	bool App::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
			m_Minimized = true;
		else
			m_Minimized = false;

		return false;
	}
	


}
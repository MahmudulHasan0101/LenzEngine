#pragma once

#include "Utils/TimeStep.h"
#include "Lenz/Window.h"
#include "Lenz/Event/Event.h"
#include "Layer.h"
#include "LayerStack.h"
//#include "Secondary/List.h"
#include "LayerStack.h"
#include "Renderer/Buffer.h"

#include <Lenz/Core.h>

extern lenz::Window* GetAppWindow();

namespace lenz
{
	class  App
	{
	public:
		App();
		virtual~App();
		void Run();

		void OnEvent(Event& e);

		static inline App& Get() { return *s_Instance; }
		static inline Window& GetWindow() { return *s_Window; }
		static inline TimeStep& GetTimeStep() { return m_TimeStep; }

		bool OnWindowClosed(WindowCloseEvent&);
		bool OnWindowResize(WindowResizeEvent&);

	protected:
		bool m_Running = true;
		bool m_Minimized = false;

		inline LayerStack& GetLayerStack() { return m_LayerStack; }
		inline void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }
		inline void PushOverlay(Layer* layer) { m_LayerStack.PushOverLay(layer); }


	private:
		 LayerStack m_LayerStack;

		static App* s_Instance;

		static Window* s_Window;
		static TimeStep m_TimeStep;
	};

}

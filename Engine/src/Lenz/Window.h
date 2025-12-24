#pragma once
#include<string>
#include<functional>

#include"Event/Event.h"
#include"Event/EventAdaptor.h"
#include "Lenz/Utils/TimeStep.h"


namespace lenz
{
	using EventCallBackFn = std::function<void(Event&)>;

	class WindowData
	{
	public:
		WindowData()
			:Width(720), Height(640), Title("Untitled Window"), VSync(true)
		{
		}
		std::string Title;
		uint32_t Width, Height;
		bool VSync;

		EventCallBackFn EventCallback;
	};

	class Window
	{
	protected:
		void OnUpdate(TimeStep& ts);

	public:
		Window();
		~Window();

		inline uint32_t GetWidth() const { return m_Data.Width; };
		inline uint32_t GetHeight() const { return m_Data.Height; };

		void SetEventCallback(const EventCallBackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enable);
		bool IsVSync() const;

		GLFWwindow* GetNativeWindow() { return m_Window; };
		WindowData& GetData() { return m_Data; };

		virtual void Init(const WindowData& props = WindowData());
		virtual void Shutdown();


		inline operator GLFWwindow* () const { return m_Window; }
	private:
		//void SetEvents();

	private:
		GLFWwindow* m_Window = nullptr;
		WindowData m_Data = WindowData();

		friend class App;
	};

}
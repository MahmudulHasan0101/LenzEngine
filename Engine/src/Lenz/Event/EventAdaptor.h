#pragma once
#include "Event.h"
#include "ApplicationEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"

#include <Lenz/Core.h>

namespace lenz
{

	class EventAdaptor
	{
	public:
		virtual bool OnEvent(Event& e)
		{
			RunOnEventFunctions(e);
			return false;
		}

	protected:
		virtual bool RunOnEventFunctions(Event& e)
		{
			EventDispatcher dispatcher(e);


			dispatcher.Dispatch<WindowCloseEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnWindowClose));
			dispatcher.Dispatch<WindowResizeEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnWindowResize));
			dispatcher.Dispatch<WindowLostFocusEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnWindowLostFocus));
			dispatcher.Dispatch<WindowMovedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnWindowMoved));
			dispatcher.Dispatch<KeyPressedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnKeyPressed));
			dispatcher.Dispatch<KeyReleasedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnKeyReleased));
			dispatcher.Dispatch<MouseButtonPressedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnMouseButtonPressed));
			dispatcher.Dispatch<MouseButtonReleasedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnMouseButtonReleased));
			dispatcher.Dispatch<MouseMovedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnMouseMoved));
			dispatcher.Dispatch<MouseScrolledEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnMouseScrolled));
			dispatcher.Dispatch<KeyTypedEvent>(LZ_BIND_EVENT_FN(EventAdaptor::OnKeyTyped));
			return false;
		}

		virtual bool OnWindowClose(WindowCloseEvent& e) { return false; }
		virtual bool OnWindowResize(WindowResizeEvent& e) { return false; }
		virtual bool OnWindowLostFocus(WindowLostFocusEvent& e) { return false; }
		virtual bool OnWindowMoved(WindowMovedEvent& e) { return false; }
		virtual bool OnKeyPressed(KeyPressedEvent& e) { return false; }
		virtual bool OnKeyReleased(KeyReleasedEvent& e) { return false; }
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& e) { return false; }
		virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent& e) { return false; }
		virtual bool OnMouseMoved(MouseMovedEvent& e) { return false; }
		virtual bool OnMouseScrolled(MouseScrolledEvent& e) { return false; }
		virtual bool OnKeyTyped(KeyTypedEvent& e) { return false; }
	};
}
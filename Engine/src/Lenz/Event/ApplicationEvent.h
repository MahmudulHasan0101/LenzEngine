#pragma once

#include <sstream>
#include "Event.h"


namespace lenz
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString()
		{
			std::stringstream ss;
			ss << "WindowResizEvent: " << m_Width << "," << m_Height;
			return ss.str();
		};

		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication);

	private:
		unsigned int m_Width, m_Height;
	};



	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication);
	};


	class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		EVENT_CLASS_TYPE(WindowLostFocus);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication);
	};

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent() {}

		EVENT_CLASS_TYPE(WindowMoved);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication);

		inline int GetX() const { return newX; }
		inline int GetY() const { return newY; }

	private:
		int newX, newY;
	};
}
#pragma once

#include "Event.h"

#include <sstream>

namespace lenz
{

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float X, float Y)
			: m_MouseX(X), m_MouseY(Y), m_MouseVelX(m_MouseX-m_PastX), m_MouseVelY(m_MouseY-m_PastY)
		{
			m_PastX = m_MouseX;
			m_PastY = m_MouseY;
		}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }
		inline float GetVelX() const { return m_MouseVelX; }
		inline float GetVelY() const { return m_MouseVelY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMoveEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryMouse | (int)EventCategory::EventCategoryInput);

		static float m_PastX;
		static float m_PastY;
	private:
		float m_MouseX, m_MouseY;
		float m_MouseVelX, m_MouseVelY;
	};



	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			:m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset;	}
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryMouse | (int)EventCategory::EventCategoryInput);

	private:
		float m_XOffset, m_YOffset;
	};




	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryMouse | (int)EventCategory::EventCategoryInput);

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}

		float m_Button;
	};




	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed);
	};


	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased);
	};



}

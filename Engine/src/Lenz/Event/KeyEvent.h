#pragma once

#include "Event.h"
#include <sstream>

namespace lenz
{
	class KeyEvent : public Event
	{
	public:

		inline int GetKeyCode() { return m_KeyCode; };

		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryKeyboard | (int)EventCategory::EventCategoryInput)

	protected:
		KeyEvent(int Keycode)
			: m_KeyCode(Keycode) {}

		int m_KeyCode;

	};


	class KeyPressedEvent : public KeyEvent
	{
	public:	
		KeyPressedEvent(int Keycode, int reapeatCount)
			: KeyEvent(Keycode), m_RepeatCount(reapeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed);

	private:
		int m_RepeatCount;

	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode)	{}

		
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleaseEvent: " << m_KeyCode << " (" << m_KeyCode << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)

	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(unsigned int keychar)
			:keychar(keychar)
		{
		}

		inline char GetKeyChar() const { return keychar; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTyped: " << keychar;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped);
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryKeyboard | (int)EventCategory::EventCategoryInput)
    
	private:
		unsigned int keychar;

	};

}
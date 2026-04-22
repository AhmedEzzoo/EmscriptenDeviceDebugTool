#pragma once
#include "Event.h"
namespace Ezzoo {

	class KeyPressedEvent : public Event
	{
	public :
		KeyPressedEvent(int key, int repeatCount) : m_KeyCode(key), m_RepeatCount(repeatCount) {}

		int GetKeyCode() const { return m_KeyCode; }
		int GetRepeatCount() const { return m_RepeatCount; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key Pressed Event : " << m_KeyCode;
			return ss.str();
		}


		EVENT_CLASS_TYPE(KeyPressed);
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	private :
		int m_KeyCode;
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(int key) : m_KeyCode(key) {}

		int GetKeyCode() const { return m_KeyCode; }
		
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key Released Event : " << m_KeyCode;
			return ss.str();
		}


		EVENT_CLASS_TYPE(KeyReleased);
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	private:
		int m_KeyCode;
		
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(int key) : m_KeyCode(key) {}

		int GetKeyCode() const { return m_KeyCode; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key Typed Event : " << m_KeyCode;
			return ss.str();
		}


		EVENT_CLASS_TYPE(KeyTyped);
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	private:
		int m_KeyCode;

	};
}
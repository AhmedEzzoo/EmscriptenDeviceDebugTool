#pragma once

#include "Event.h"

namespace Ezzoo {

	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float xPos, float yPos) : m_Xpos(xPos), m_Ypos(yPos) {}

		float GetXPos() const { return m_Xpos; }
		float GetYPos() const { return m_Ypos; }

		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton);
		
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent" << "X: " << m_Xpos << ",Y: " << m_Ypos;
			return ss.str();
		}
	private:
		float m_Xpos;
		float m_Ypos;
	};

	class MouseScrolledEvent : public Event
	{
	public :
		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset){}

		float GetXoffset() const { return m_XOffset; }
		float GetYoffset() const { return m_YOffset; }

		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategoryMouse);

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent" << "X_Offset: " << m_XOffset << ",Y_Offset: " << m_YOffset;
			return ss.str();
		}

	private :
		float m_XOffset{0.0};
		float m_YOffset{0.0};
	};


	class MousePressedEvent : public Event
	{
	public:
		MousePressedEvent(int button) : m_Button(button){}

		int GetMouseButton() const { return m_Button; }


		EVENT_CLASS_TYPE(MouseButtonPressed);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton);
		

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MousePressedEvent" << "Button: " << m_Button; 
			return ss.str();
		}

	private:
		int m_Button;
	};

	class MouseReleasedEvent : public Event
	{
	public:
		MouseReleasedEvent(int button) : m_Button(button) {}

		int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_TYPE(MouseButtonReleased);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton);
		

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseReleasedEvent" << "Button: " << m_Button;
			return ss.str();
		}

	private:
		int m_Button;
	};
}
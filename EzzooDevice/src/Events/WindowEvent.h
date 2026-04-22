#pragma once
#include "Event.h"

namespace Ezzoo {

	class WindowCloseEvent : public Event
	{
	public :
	
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Close Event";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(unsigned int width, unsigned int height) : m_Width(width), m_Height(height) {} ;

		unsigned int GetWindowWidth() const { return m_Width; }
		unsigned int GetWindowHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Resize Event: Width: " << m_Width << "Height: " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	private :
		unsigned int m_Width;
		unsigned int m_Height;
	};

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(int xPos,  int yPos) : m_XPos(xPos), m_YPos(yPos) {};

		int GetWindowXPos() const { return m_XPos; }
		int GetWindowYPos() const { return m_YPos; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Moved Event: xPos: " << m_XPos << "yPos: " << m_YPos;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowMoved);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	private:
		int m_XPos;
		int m_YPos;
	};
}
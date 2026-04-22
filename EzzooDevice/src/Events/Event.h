#pragma once
#include "ezpch.h"
#include "Core/Core.h"

namespace Ezzoo {

	enum class EventType
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowLostFocus,
		WindowMoved,
		AppTick,
		AppUpdate,
		AppRender,
		KeyPressed,
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled

	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};
#define EVENT_CLASS_TYPE(type)                          \
    static EventType GetStaticType()                    \
    {                                                   \
        return EventType::type;                         \
    }                                                   \
    virtual EventType GetEventType() const override     \
    {                                                   \
        return GetStaticType();                         \
    }                                                   \
    virtual const char *GetEventName() const override   \
    {                                                   \
        return #type;                                   \
    }

#define EVENT_CLASS_CATEGORY(category)                  \
    virtual int GetEventCategory() const override       \
    {                                                   \
        return category;                                \
    }

	class Event
	{
		//friend class EventDispatcher;
	public :

		virtual EventType GetEventType() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual int GetEventCategory() const = 0;
		virtual std::string ToString() const { return GetEventName(); }
		
		inline bool IsInCategory(const EventCategory& category) { return GetEventCategory() & category; }


	public :
		bool m_Handeled{ false };

	};

	class EventDispatcher
	{
		template <typename T>
		using EventFn = std::function<bool(T&)>;
	public :
			
		EventDispatcher(Event& event) : m_Event(event) {};
		~EventDispatcher() = default;

		template <typename T>
		bool Dispatch(EventFn<T> fn)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handeled = fn(static_cast<T&>(m_Event));
				return true;
			}

			return false;
		}


	private :
		Event& m_Event;
	};

	inline std::ostream& operator<< (std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

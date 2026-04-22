#pragma once


#include "Events/Event.h"

namespace Ezzoo {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned Height;

		WindowProps(const std::string& title = "Ezzoo", unsigned int width = 900, unsigned int height = 600) 
			: Title(title), Width(width), Height(height){}
	};

	class Window
	{
	public:
		using EventCallBack = std::function<void(Event &)>;


		virtual ~Window (){}

		
		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual void SetEventCallBack(const EventCallBack& callBack) = 0;

		virtual void SetVSync(bool enable)  = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());

	};
}
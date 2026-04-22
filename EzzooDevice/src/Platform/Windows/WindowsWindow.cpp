#include "ezpch.h"

#include "WindowsWindow.h"
#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Ezzoo {

	static bool s_GLFWInit{ false };

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}
	WindowsWindow::~WindowsWindow()
	{
		ShutDown();
		delete m_Context;
	}

	void WindowsWindow::Init(const WindowProps& props) 
	{
		m_Data.Height = props.Height;
		m_Data.Width = props.Width;
		m_Data.Title = props.Title;

		EZZOO_CORE_INFO("Window Data-> Width: {0}, Height: {1}, Title: {2}", m_Data.Width, m_Data.Height, m_Data.Title);

		if (!s_GLFWInit)
		{
			int success = glfwInit();
			if (!success) EZZOO_CORE_ERROR("GLFW Could not intialize!!!!");
			s_GLFWInit = true;
		}

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		glfwSetWindowUserPointer(m_Window, &m_Data);
		#ifndef __EMSCRIPTEN__
			SetVSync(true);
		#endif

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.CallBack(event);
			});
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				WindowResizedEvent event((unsigned int)width, (unsigned int)height);
				data.CallBack(event);
			});
		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowMovedEvent event(xpos, ypos);
				data.CallBack(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
				switch (action)
				{
					case GLFW_PRESS :
					{
						MousePressedEvent event(button);
						data.CallBack(event);
						break;
					}
					case GLFW_RELEASE :
						MouseReleasedEvent event(button);
						data.CallBack(event);
						break;
				}
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMoveEvent event((float)xpos, (float)ypos);
				data.CallBack(event);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xoffset, (float)yoffset);
				data.CallBack(event);
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(codepoint);
				data.CallBack(event);
			});

		glfwSetKeyCallback(m_Window,[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action) 
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.CallBack(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.CallBack(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 10);
						data.CallBack(event);
						break;
					}
				}
			});
	}
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	unsigned int WindowsWindow::GetWidth() const
	{
		return m_Data.Width;
	}
	unsigned int WindowsWindow::GetHeight() const
	{
		return m_Data.Height;
	}

	void WindowsWindow::SetVSync(bool enable)
	{
		if (enable)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
		m_Data.VSync = enable;
	}
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync ? true : false;
	}

	void WindowsWindow::ShutDown()
	{
		glfwDestroyWindow(m_Window);
	}
}
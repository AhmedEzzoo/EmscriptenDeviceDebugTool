#pragma once

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;
namespace Ezzoo {

	class OpenGLContext : public GraphicsContext
	{
	public :

		OpenGLContext(GLFWwindow* windowHandle);
		~OpenGLContext();
		virtual void Init() override;
		virtual void SwapBuffers() override ;


	private :
		GLFWwindow*  m_WindowHandle;
	};
}
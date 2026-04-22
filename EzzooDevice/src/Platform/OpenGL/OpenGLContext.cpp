#include "ezpch.h"
#include "OpenGLContext.h"


#ifndef __EMSCRIPTEN__

	#include "glad/glad.h"
#else 
	#include "GLES3/gl3.h"
	#include "emscripten.h"
#endif


#include <GLFW/glfw3.h>
namespace Ezzoo {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		
	}
	OpenGLContext::~OpenGLContext()
	{
		EZZOO_CORE_INFO("OpenGLContext Destructor Called");
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		#ifndef __EMSCRIPTEN__ 
			int status =  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		
		if (status)
		{
			EZZOO_CORE_INFO("OpenGL Vendor is: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
			EZZOO_CORE_INFO("OpenGL Renderer is: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
			EZZOO_CORE_INFO("OpenGL Version is: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		}
		#else 
			EZZOO_CORE_INFO("OpenGL Vendor is: {0}", (const char*)(glGetString(GL_VENDOR)));
			EZZOO_CORE_INFO("OpenGL Renderer is: {0}", (const char*)(glGetString(GL_RENDERER)));
			EZZOO_CORE_INFO("OpenGL Version is: {0}", (const char*)(glGetString(GL_VERSION)));
		#endif

	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
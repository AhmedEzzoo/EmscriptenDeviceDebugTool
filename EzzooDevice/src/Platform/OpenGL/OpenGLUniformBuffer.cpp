#include "ezpch.h"


#include "OpenGLUniformBuffer.h"

#ifndef __EMSCRIPTEN__
	#include "glad/glad.h"
#else
	#include "GLES3/gl3.h"
	#include "emscripten.h"
#endif


namespace Ezzoo {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		#ifndef __EMSCRIPTEN__
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
		#endif
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		#ifndef __EMSCRIPTEN__
		glDeleteBuffers(1, &m_RendererID);
		#endif
	}


	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		#ifndef __EMSCRIPTEN__
		glNamedBufferSubData(m_RendererID, offset, size, data);
		#endif
	}


}
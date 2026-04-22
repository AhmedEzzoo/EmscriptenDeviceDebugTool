#include "ezpch.h"

#include "OpenGLBuffer.h"

#ifndef __EMSCRIPTEN__
	#include <glad/glad.h>
#else 
	#include "GLES3/gl3.h"
	#include "emscripten.h"
#endif
namespace Ezzoo {

	///////////////////VERTEX BUFFER//////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}


	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind()  const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}
	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void  OpenGLVertexBuffer::SetData(const void* data, uint32_t dataSize)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
	}

	void OpenGLVertexBuffer::ClearData(const void* data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		//glClearBufferData(GL_ARRAY_BUFFER, GL_RGBA8, GL_RGBA, GL_FLOAT, data);
	}

	const BufferLayout& OpenGLVertexBuffer::GetLayout() const  
	{
		return m_Layout;
	}
	
	void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;
	}


	///////////////////INDEX BUFFER//////////////////////////
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indecies, uint32_t count) : m_Count(count)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indecies, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	void OpenGLIndexBuffer::Bind()  const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

	}
	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
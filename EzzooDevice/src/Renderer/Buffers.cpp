#include "ezpch.h"

#include "Buffers.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLBuffer.h"


namespace Ezzoo {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	EZZOO_CORE_ERROR("None Is not type of Renderer"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(size);
		}

		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	EZZOO_CORE_ERROR("None Is not type of Renderer"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}


		return nullptr;
	}


	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indecies, uint32_t count)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	EZZOO_CORE_ERROR("None Is not type of Renderer"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLIndexBuffer>(indecies, count);
		}


		return nullptr;
	}
}
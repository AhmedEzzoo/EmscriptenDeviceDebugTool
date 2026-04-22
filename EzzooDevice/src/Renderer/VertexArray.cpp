#include "ezpch.h"

#include "VertexArray.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Ezzoo {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:	EZZOO_CORE_ASSERT(false, "None is not Renderer Type!!") return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();
		}

		EZZOO_CORE_ASSERT(false, "Type is not available !!");
		return nullptr;
	}
}
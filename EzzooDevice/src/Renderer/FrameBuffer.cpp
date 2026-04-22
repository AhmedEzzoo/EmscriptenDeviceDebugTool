//#include "FrameBuffer.h"

#include "ezpch.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Ezzoo {


	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{

		case RendererAPI::API::None:  EZZOO_CORE_ASSERT(false, "Not Proper Renderer") return nullptr;

		case RendererAPI::API::OpenGL: return CreateRef<OpenGLFrameBuffer>(spec);

		}

		return nullptr;
	}


}
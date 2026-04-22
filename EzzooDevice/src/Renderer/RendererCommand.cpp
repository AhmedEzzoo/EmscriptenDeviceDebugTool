#include "ezpch.h"

#include "RendererCommand.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Ezzoo {

	RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRenderer;
}
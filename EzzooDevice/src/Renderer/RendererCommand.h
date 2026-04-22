#pragma once

#include "glm/glm.hpp"
#include "VertexArray.h"
#include "RendererAPI.h"

namespace Ezzoo {

	class RendererCommand
	{
	public :

		static void Init()
		{
			s_RendererAPI->Init();
		}

		static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewPort(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		static void ClearColor()
		{
			s_RendererAPI->ClearColor();
		}

		static inline void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static inline void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawLine(vertexArray, count);
		}
		static inline void SetWidth(float width)
		{
			s_RendererAPI->SetWidth(width);
		}

	private :
		static RendererAPI* s_RendererAPI;
	};
}
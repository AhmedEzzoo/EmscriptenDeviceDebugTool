#pragma once

#include "VertexArray.h"
#include "glm/glm.hpp"


namespace Ezzoo {

	class RendererAPI
	{
	public:



		enum class API
		{
			None = 0,
			OpenGL = 1
		};
	public:

		virtual void Init() = 0;

		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void ClearColor() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;
		virtual void DrawLine(const Ref<VertexArray>& vertexArray, uint32_t count) = 0 ;
		virtual void SetWidth(float width) = 0;
		inline static API GetAPI() { return s_RendererAPI; }
	private:
		static API s_RendererAPI;
	};
}

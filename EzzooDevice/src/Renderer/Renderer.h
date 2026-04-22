#pragma once

#include "VertexArray.h"

#include "RendererAPI.h"

#include "OrthoGraphicCamera.h"

#include "Shader.h"

namespace Ezzoo {

	class Renderer
	{
	public :
		static void Init();
		static void BeginScene(OrthoGraphicCamera& camera) ;
		static void EndScene();
		static void OnWindowResize(uint32_t width, uint32_t height);


		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader) ;

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private :
		struct SceneData
		{
			glm::mat4 ViewProjection;
		};

		static SceneData* s_Data;

	};
}

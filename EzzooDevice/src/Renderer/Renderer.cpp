#include "ezpch.h"

#include "Renderer.h"
#include "RendererCommand.h"
#include "Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Ezzoo {

	Renderer::SceneData* Renderer::s_Data = new SceneData();

	void Renderer::Init()
	{
		RendererCommand::Init();

		//Renderer2D::Init();
	}

	void Renderer::BeginScene(OrthoGraphicCamera& camera)
	{
		s_Data->ViewProjection =  camera.GetViewProjection();

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RendererCommand::SetViewPort(0, 0, width, height);
	}

	void Renderer::EndScene()
	{

	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		vertexArray->Bind();
		//std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_Data->ViewProjection);
		RendererCommand::DrawIndexed(vertexArray, 1);
	}
}
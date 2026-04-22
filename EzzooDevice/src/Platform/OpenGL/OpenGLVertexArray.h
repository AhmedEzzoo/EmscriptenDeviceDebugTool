#pragma once
#include "Renderer/Buffers.h"
#include "Renderer/VertexArray.h"
namespace Ezzoo {

	class OpenGLVertexArray : public VertexArray
	{
	public :

		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)  override;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;
		
		virtual void Test() override;

	private :
		uint32_t m_RendererID{0};
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		uint32_t m_VertwxBufferIndex = 0;


	};
}
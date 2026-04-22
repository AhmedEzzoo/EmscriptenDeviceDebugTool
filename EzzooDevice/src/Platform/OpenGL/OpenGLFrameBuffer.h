#pragma once

#include "Renderer/FrameBuffer.h"

namespace Ezzoo {

	class OpenGLFrameBuffer : public FrameBuffer
	{
	public :

		OpenGLFrameBuffer(const FrameBufferSpecification& specs);
		virtual ~OpenGLFrameBuffer();


		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind() override ;

		virtual void Resize(uint32_t width, uint32_t height) override ;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override ;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override ;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override ;

		virtual const FrameBufferSpecification& GetSpecification() const override ;


	private :

		FrameBufferSpecification m_Specifications;
		uint32_t m_RendererID = 0;

		std::vector<uint32_t> m_ColorAttachments; // uint32_t m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		FrameBufferTextureSpecification m_DepthAttachmentSpecification = FrameBufferTextureFormate::None;
	};


}
#pragma once

#include <vector>

#include "Core/Core.h"


namespace Ezzoo{


	enum class FrameBufferTextureFormate
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};


	struct FrameBufferTextureSpecification
	{

		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification (FrameBufferTextureFormate formate)
			: TextureFormate(formate)
		{}

		FrameBufferTextureFormate TextureFormate = FrameBufferTextureFormate::None;
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification (std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments)
		{}

		std::vector<FrameBufferTextureSpecification> Attachments;
	};

	struct FrameBufferSpecification
	{
		uint32_t  Width = 0, Height = 0;
		FrameBufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};


	class FrameBuffer
	{
	public :

		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
	
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);

	};
}
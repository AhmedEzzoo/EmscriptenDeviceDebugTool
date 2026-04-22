#pragma once


#include "Renderer/Texture.h"

#ifndef __EMSCRIPTEN__
	#include "glad/glad.h"
#else
	#include "GLES3/gl3.h"
	#include "emscripten.h"
#endif

#include "stb_image.h"

namespace Ezzoo {

	class  OpenGLTexture2D : public Texture2D
	{
	public :

		OpenGLTexture2D(const TextureSpecification& specs, Buffer data = Buffer());

		OpenGLTexture2D(const std::string& filepath );

		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; };
		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint8_t slot = 0) const override;


		virtual uint32_t GetRendererID() const override { return m_RendererID; };

		virtual uint64_t GetTextureSize() const override { return m_Width * m_Height * 4; };


		virtual bool operator== (const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}


	private :
		uint32_t m_RendererID;

		uint32_t m_Width{0};
		uint32_t m_Height{0};

		std::string m_Path;

		TextureSpecification m_Specification;

		GLenum m_InternalFormat, m_DataFormat;
	};
}
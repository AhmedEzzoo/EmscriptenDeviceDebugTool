#include "ezpch.h"

#include "OpenGLTexture2D.h"
#include "Core/Buffer.h"

#include "Renderer/Texture.h"



namespace Ezzoo {


	static GLenum EzzooFormatToGLFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGB8:  return GL_RGB8;
		case ImageFormat::RGBA8: return GL_RGBA8;
		
		}

		EZZOO_CORE_ERROR("Image Format Not IOmplemented");
		return 0;
	}
	static GLenum EzzooImageFormatToGLDataFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGB8:  return GL_RGB;
		case ImageFormat::RGBA8: return GL_RGBA;
		}

		EZZOO_CORE_ASSERT(false, "Wrong Format Image");
		return 0;
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specs, Buffer data)
		: m_Specification(specs), m_Width(specs.Width), m_Height(specs.Height)
	{
		m_InternalFormat = EzzooFormatToGLFormat(m_Specification.Format);
		m_DataFormat = EzzooImageFormatToGLDataFormat(m_Specification.Format);

		#ifndef __EMSCRIPTEN__
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data.Data)
		{
			SetData(data.Data, data.Size);
		}
		#endif
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath) : m_Path(filepath)
	{
		#ifndef __EMSCRIPTEN__
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		data.Data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

		m_Width = width;
		m_Height = height;


		GLenum internalFormat = 0, dataFormat = 0;

		if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		glTextureStorage2D(m_RendererID, 1, internalFormat , m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data.Data);
		

		stbi_image_free(data.Data);
		#endif
	}



	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		#ifndef __EMSCRIPTEN__
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		#endif
	}

	void OpenGLTexture2D::Bind(uint8_t slot) const
	{
		#ifndef __EMSCRIPTEN__
		glBindTextureUnit(slot, m_RendererID);
		#endif
	}
	

	
}
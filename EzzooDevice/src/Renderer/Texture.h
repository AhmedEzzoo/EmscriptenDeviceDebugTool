#pragma once

#include "Core/Core.h"
#include "Core/Buffer.h"

#include <filesystem>

namespace Ezzoo {

	enum class ImageFormat
	{
		None = 0,
		RGB,
		RGB8,
		RGBA,
		RGBA8
	};


	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;

		ImageFormat Format = ImageFormat::RGBA8;
	};


	class Texture
	{
	public :
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint8_t slot = 0) const = 0;

		virtual bool operator== (const Texture& other) const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual uint64_t GetTextureSize() const = 0;


	private :

	};


	class Texture2D : public Texture
	{ 
		public :
			virtual ~Texture2D() = default;

			static Ref<Texture2D> CreateTexture2D(const TextureSpecification& specs, Buffer data );
			static Ref<Texture2D> CreateTexture2D(const std::string& filepath);


			/*above strategy as follow 
			  you can get Asset type using static or object

			 template <typename T>
			 void Lol (T t)
			 {
				T::GetStaticAssetType();
				or  t.GetAssetType()
			 }*/
		
			
	};
}
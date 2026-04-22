#pragma once

#include "ezpch.h"

#include <stdint.h>
#include <cstring>
namespace Ezzoo {

	struct Buffer
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;


		Buffer() = default;

		Buffer(uint64_t size)
		{
			Allocate(size);
		}

		~Buffer() = default;


		void Release()
		{
			delete Data;
			Data = nullptr;

			Size = 0;
		}



		void Allocate(uint64_t size)
		{
			Release();
			

			Data = (uint8_t*)malloc(size);
			Size = size;
		}

		static Buffer Copy(const Buffer& other)
		{
			Buffer buffer;
			buffer.Size = other.Size;

			memcpy(buffer.Data, other.Data, other.Size);

			return buffer;

		}

		template <typename T>
		T* As()
		{
			return (T*)Data;
		}

	};



}


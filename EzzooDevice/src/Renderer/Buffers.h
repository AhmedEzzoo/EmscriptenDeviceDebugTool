#pragma once

#include "Core/Core.h"

namespace Ezzoo {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat2, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t GetDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:			return 4;
		case ShaderDataType::Float2:		return 4 * 2;
		case ShaderDataType::Float3:		return 4 * 3;
		case ShaderDataType::Float4:		return 4 * 4;
		case ShaderDataType::Mat2:			return 4 * 2 * 2;
		case ShaderDataType::Mat3:			return 4 * 3 * 3;
		case ShaderDataType::Mat4:			return 4 * 4 * 4;
		case ShaderDataType::Int:			return 4;
		case ShaderDataType::Int2:			return 4 * 2;
		case ShaderDataType::Int3:			return 4 * 3;
		case ShaderDataType::Int4:			return 4 * 4;
		case ShaderDataType::Bool:			return 1;

		}

		EZZOO_CORE_ASSERT(false, "Type is Not Supported");
		return 0;
	}

	struct BufferElement
	{

		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalize = false) :
			Type(type), Name(name),  Normalized(normalize), Size(GetDataTypeSize(type)), Offset(0)
		{
		}

		inline uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:			return 1;
			case ShaderDataType::Float2:		return 2;
			case ShaderDataType::Float3:		return 3;
			case ShaderDataType::Float4:		return 4;
			case ShaderDataType::Mat2:			return 2 * 2;
			case ShaderDataType::Mat3:			return 3 * 3;
			case ShaderDataType::Mat4:			return 4 * 4;
			case ShaderDataType::Int:			return 1;
			case ShaderDataType::Int2:			return 2;
			case ShaderDataType::Int3:			return 3;
			case ShaderDataType::Int4:			return 4;
			case ShaderDataType::Bool:			return 1;

			}

			//EZZOO_CORE_ASSERT(false, "Type is Not Supported");
			return 0;
		}
	};



	class BufferLayout
	{
	public:

		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements)
		{
			CalculateOffset();
		}

		uint32_t GetStrid() const { return m_StrID; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }


	private:
		inline void CalculateOffset()
		{
			unsigned int offset = 0;
			m_StrID = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_StrID += element.Size;
			}
		}


	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_StrID = 0;
	};


	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t dataSize) = 0;
		virtual void ClearData(const void* data) = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indecies, uint32_t count);

	};
}

#include "ezpch.h"

#include "Renderer2D.h"
#include "RendererCommand.h"

#include "VertexArray.h"

#include "UniformBuffer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"


namespace Ezzoo {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextCoords;
		float TextSlot = 0.0f;
		float TilingFactor = 1.0f;
		int EntityID;
	};
	
	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;
		
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	
		int EntityID;
	};

	struct Renderer2DData
	{

		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndecies = MaxQuads * 6;

		static const uint32_t MaxTexturesSlots = 32;

		//Quad
		Ref<VertexArray> RendererVA;
		Ref<VertexBuffer> RendererVB;
		Ref<Shader> QuadShader;
		uint32_t QuadIndciesCount = 0;
		QuadVertex* QuadVertexCurrentPtr = nullptr;

		QuadVertex* QuadVertexBase = nullptr;
		QuadVertex* QuadVertexPtr = nullptr;

		//Circle
		Ref<VertexArray> CircleVA;
		Ref<VertexBuffer> CircleVB;
		Ref<Shader> CircleShader;
		uint32_t CircleIndciesCount = 0;
		CircleVertex* CircleVertexCurrentPtr = nullptr;

		CircleVertex* CircleVertexBase = nullptr;
		CircleVertex* CircleVertexPtr = nullptr;
		
		//Line
		Ref<VertexArray> LineVA;
		Ref<VertexBuffer> LineVB;
		Ref<Shader> LineShader;
		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexCurrentPtr = nullptr;

		LineVertex* LineVertexBase = nullptr;
		LineVertex* LineVertexPtr = nullptr;

		float LineWidth = 2.0f;

		//WhiteTexture
		Ref<Texture2D> WhiteTexture;


		std::array<Ref<Texture2D>, MaxTexturesSlots> Textures;

		uint32_t TextureIndex = 1;

		Renderer2D::RendererStats stats;

		glm::vec4 QuadPosition[4];

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		
	};


	static Renderer2DData s_Data;

	Renderer2D::RendererStats& Renderer2D::GetStats() { return s_Data.stats; }

	void Renderer2D::Init()
	{
	

		//Quad
		s_Data.RendererVA = VertexArray::Create();
		s_Data.RendererVA->Bind();
		s_Data.RendererVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.RendererVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position"}, 
				{ ShaderDataType::Float4, "a_Color"}, 
				{ ShaderDataType::Float2, "a_TextCoord"},
				{ ShaderDataType::Float , "a_TextSlot"},
				{ ShaderDataType::Float , "a_TilingFactor"},
				{ ShaderDataType::Int, "a_EntityID" }

			});



		s_Data.RendererVA->AddVertexBuffer(s_Data.RendererVB);
		s_Data.QuadVertexBase = new QuadVertex[s_Data.MaxVertices];
		uint32_t* Quadindecies = new uint32_t[s_Data.MaxIndecies];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndecies; i += 6)
		{
			Quadindecies[i + 0] = offset + 0;
			Quadindecies[i + 1] = offset + 1;
			Quadindecies[i + 2] = offset + 2;

			Quadindecies[i + 3] = offset + 2;
			Quadindecies[i + 4] = offset + 3;
			Quadindecies[i + 5] = offset + 0;

			offset += 4;
		}
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Quadindecies, s_Data.MaxIndecies / sizeof(uint32_t));
		s_Data.RendererVA->SetIndexBuffer(indexBuffer);

		delete[] Quadindecies;


		//Circle
		s_Data.CircleVA = VertexArray::Create();
		s_Data.CircleVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVB->SetLayout({
				{ ShaderDataType::Float3, "a_WorldPosition"	},
				{ ShaderDataType::Float3, "a_LocalPosition"	},
				{ ShaderDataType::Float4, "a_Color"			},
				{ ShaderDataType::Float , "a_Thickness"		},
				{ ShaderDataType::Float , "a_Fade"			},
				{ ShaderDataType::Int,	  "a_EntityID"		}

			});
		s_Data.CircleVA->AddVertexBuffer(s_Data.CircleVB);
		s_Data.CircleVA->SetIndexBuffer(indexBuffer);
		s_Data.CircleVertexBase = new CircleVertex[s_Data.MaxVertices];

		//Line
		s_Data.LineVA = VertexArray::Create();
		s_Data.LineVA->Bind();
		s_Data.LineVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVB->SetLayout({
				{ ShaderDataType::Float3, "a_Position"	},
				{ ShaderDataType::Float4, "a_Color"		},
				{ ShaderDataType::Int,	  "a_EntityID"	}

			});
		s_Data.LineVA->AddVertexBuffer(s_Data.LineVB);
		s_Data.LineVertexBase = new LineVertex[s_Data.MaxVertices];


		//White Texture
		//Buffer data(sizeof(uint32_t));

		s_Data.WhiteTexture = Texture2D::CreateTexture2D(TextureSpecification(), Buffer());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t sampler[s_Data.MaxTexturesSlots];
		for (uint32_t i = 0; i < s_Data.MaxTexturesSlots; i++)
			sampler[i] = i;


		s_Data.QuadShader = Shader::Create("Resources/Shaders/QuadShader.glsl");
		s_Data.CircleShader = Shader::Create("Resources/Shaders/CircleShader.glsl");
		s_Data.LineShader = Shader::Create("Resources/Shaders/LineShader.glsl");

		s_Data.Textures[0] = s_Data.WhiteTexture;
		 



		s_Data.QuadPosition[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadPosition[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadPosition[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadPosition[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
		
	}

	void Renderer2D::ShutDown()
	{
		delete[] s_Data.QuadVertexBase;
	}

	void Renderer2D::BeginScene(const OrthoGraphicCamera& camera)
	{
		//s_Data.ViewProjection = camera.GetViewProjection();

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjection());

		StartBatch();
		
	}
	
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
		
	}
	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{

		{
			if (s_Data.QuadVertexCurrentPtr != nullptr)
			{
				bool IsdataToClear = (uint32_t*)s_Data.QuadVertexCurrentPtr == (uint32_t*)s_Data.QuadVertexPtr ? false : true;

				if (IsdataToClear)
				{
					s_Data.RendererVB->ClearData(s_Data.QuadVertexPtr);
					s_Data.QuadVertexCurrentPtr = s_Data.QuadVertexPtr;
				}
			}
			else
				s_Data.QuadVertexCurrentPtr = s_Data.QuadVertexPtr;
		}
		{
			if (s_Data.CircleVertexCurrentPtr != nullptr)
			{
				bool IsdataToClear = (uint32_t*)s_Data.CircleVertexCurrentPtr == (uint32_t*)s_Data.CircleVertexPtr ? false : true;

				if (IsdataToClear)
				{
					s_Data.CircleVB->ClearData(s_Data.CircleVertexPtr);
					s_Data.CircleVertexCurrentPtr = s_Data.CircleVertexPtr;
				}
			}
			else
				s_Data.CircleVertexCurrentPtr = s_Data.CircleVertexPtr;
		}

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndciesCount = 0;
		s_Data.QuadVertexPtr = s_Data.QuadVertexBase;

		s_Data.CircleIndciesCount = 0;
		s_Data.CircleVertexPtr = s_Data.CircleVertexBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexPtr = s_Data.LineVertexBase;

		s_Data.TextureIndex = 1;
	}

	void Renderer2D::Flush()
	{
		
			if (s_Data.QuadIndciesCount)
			{

				uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexPtr - (uint8_t*)s_Data.QuadVertexBase);
				s_Data.RendererVB->SetData(s_Data.QuadVertexBase, dataSize);

				for (uint32_t i = 0; i < s_Data.TextureIndex; i++)
					s_Data.Textures[i]->Bind(i);

				s_Data.QuadShader->Bind();
				RendererCommand::DrawIndexed(s_Data.RendererVA, s_Data.QuadIndciesCount);

				s_Data.stats.DrawCalls++;
			}

		
			if (s_Data.CircleIndciesCount)
			{
				uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexPtr - (uint8_t*)s_Data.CircleVertexBase);
				s_Data.CircleVB->SetData(s_Data.CircleVertexBase, dataSize);

				s_Data.CircleShader->Bind();
				RendererCommand::DrawIndexed(s_Data.CircleVA, s_Data.CircleIndciesCount);
				s_Data.stats.DrawCalls++;
			}


			if (s_Data.LineVertexCount)
			{
				uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexPtr - (uint8_t*)s_Data.LineVertexBase);
				s_Data.LineVB->SetData(s_Data.LineVertexBase, dataSize);

				RendererCommand::SetWidth(s_Data.LineWidth);


				s_Data.LineShader->Bind();
				RendererCommand::DrawLine(s_Data.LineVA, s_Data.LineVertexCount);
				s_Data.stats.DrawCalls++;
			}
		
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 1.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		const float texIndex = 0.0f;
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		s_Data.QuadVertexPtr->Position = position;
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;


		s_Data.QuadIndciesCount += 6;
		s_Data.stats.QuadCount++;

		/*s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetFloat4("u_Color", color);
		
		s_Data.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.QuadShader->SetMat4("u_Transform", transform);

		s_Data.RendererVA->Bind();
		RendererCommand::DrawIndexed(s_Data.RendererVA);*/
	}


	void  Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 1.0f }, size, texture);
	}


	void  Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
		float textIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureIndex; i++)
		{
			if (*s_Data.Textures[i].get() == *texture.get())
			{
				textIndex = (float)i;
				break;
			}
		}

		if (textIndex == 0.0f)
		{
			textIndex = (float)s_Data.TextureIndex;
			s_Data.Textures[s_Data.TextureIndex] = texture;
			s_Data.TextureIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);


		s_Data.QuadVertexPtr->Position = position;
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x + size.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = { position.x, position.y + size.y, position.z };
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexPtr++;


		s_Data.QuadIndciesCount += 6;
		s_Data.stats.QuadCount++;
		
		
		/*s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetFloat4("u_Color", glm::vec4(1.0f));
		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.QuadShader->SetMat4("u_Transform", transform);

		s_Data.RendererVA->Bind();
		RendererCommand::DrawIndexed(s_Data.RendererVA);*/
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		const float texIndex = 0.0f;
		const float tiligFactor = 1.0;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[0];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr->EntityID = entityID;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[1];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr->EntityID = entityID;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[2];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr->EntityID = entityID;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[3];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr->EntityID = entityID;
		s_Data.QuadVertexPtr++;


		s_Data.QuadIndciesCount += 6;
		s_Data.stats.QuadCount++;

	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, float tiligFactor, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 1.0f }, size, rotation, tiligFactor, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, float tiligFactor, const glm::vec4& color)
	{

		const float texIndex = 0.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.QuadVertexPtr->Position = transform* s_Data.QuadPosition[0];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[1];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[2];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[3];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = texIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;


		s_Data.QuadIndciesCount += 6;
		s_Data.stats.QuadCount++;
	#if 0

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetFloat4("u_Color", color);

		s_Data.QuadShader->SetFloat("u_TilingFactor", tiligFactor);

		s_Data.WhiteTexture->Bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.QuadShader->SetMat4("u_Transform", transform);

		s_Data.RendererVA->Bind();
		RendererCommand::DrawIndexed(s_Data.RendererVA);
	#endif
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, float tiligFactor, const Ref<Texture2D>& texture)
	{
		DrawRotatedQuad({ position.x, position.y, 1.0f }, size, rotation, tiligFactor, texture);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, float tiligFactor, const Ref<Texture2D>& texture)
	{

		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float textIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureIndex; i++)
		{
			if (*s_Data.Textures[i].get() == *texture.get())
			{
				textIndex = (float)i;
				break;
			}
		}

		if (textIndex == 0.0f)
		{
			textIndex = (float)s_Data.TextureIndex;
			s_Data.Textures[s_Data.TextureIndex] = texture;
			s_Data.TextureIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[0];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[1];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 0.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[2];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 1.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;

		s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[3];
		s_Data.QuadVertexPtr->Color = color;
		s_Data.QuadVertexPtr->TextCoords = { 0.0f, 1.0f };
		s_Data.QuadVertexPtr->TextSlot = textIndex;
		s_Data.QuadVertexPtr->TilingFactor = tiligFactor;
		s_Data.QuadVertexPtr++;


		s_Data.QuadIndciesCount += 6;
		s_Data.stats.QuadCount++;



	#if 0
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetFloat4("u_Color", glm::vec4(1.0f));
		s_Data.QuadShader->SetFloat("u_TilingFactor", tiligFactor);

		texture->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f }) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		s_Data.QuadShader->SetMat4("u_Transform", transform);

		s_Data.RendererVA->Bind();
		RendererCommand::DrawIndexed(s_Data.RendererVA);

	#endif
	}

	void Renderer2D::FlusAndReset()
	{

		EndScene();

		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const glm::vec4& color,  float tilingFactor, int entityID)
	{
		//glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		const size_t quadVertexCount = 4;
		float textIndex = 0.0f;
		const glm::vec2 textCoord[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };



		for (uint32_t i = 1; i < s_Data.TextureIndex; i++)
		{
			if (*s_Data.Textures[i].get() == *texture.get())
			{
				textIndex = (float)i;
				break;
			}
		}

		if (textIndex == 0.0f)
		{
			if (s_Data.QuadIndciesCount >= s_Data.MaxIndecies)
				FlusAndReset();

			textIndex = (float)s_Data.TextureIndex;
			s_Data.Textures[s_Data.TextureIndex] = texture;
			s_Data.TextureIndex++;
		}

		

		for (size_t i = 0; i < quadVertexCount; i++)
		{

			s_Data.QuadVertexPtr->Position = transform * s_Data.QuadPosition[i];
			s_Data.QuadVertexPtr->Color = color;
			s_Data.QuadVertexPtr->TextCoords = textCoord[i];
			s_Data.QuadVertexPtr->TextSlot = textIndex;
			s_Data.QuadVertexPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexPtr->EntityID = entityID; 
			s_Data.QuadVertexPtr++;
		}


		s_Data.QuadIndciesCount += 6;
		s_Data.stats.QuadCount++;


	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const SpriteRendererComponent& component, int entityID)
	{
		
			DrawQuad(transform, component.Color, entityID);
	
	}


	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexPtr->WorldPosition = transform * s_Data.QuadPosition[i];
			s_Data.CircleVertexPtr->LocalPosition = s_Data.QuadPosition[i] * 2.0f;
			s_Data.CircleVertexPtr->Color = color;
			s_Data.CircleVertexPtr->Thickness = thickness;
			s_Data.CircleVertexPtr->Fade = fade;
			s_Data.CircleVertexPtr->EntityID = entityID;
			s_Data.CircleVertexPtr++;
		}


		s_Data.CircleIndciesCount += 6;
		s_Data.stats.QuadCount++;
	}



	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{

	
		s_Data.LineVertexPtr->Position = p0;
		s_Data.LineVertexPtr->Color = color;
		s_Data.LineVertexPtr->EntityID = entityID;
		s_Data.LineVertexPtr++;
	

		s_Data.LineVertexPtr->Position = p1;
		s_Data.LineVertexPtr->Color = color;
		s_Data.LineVertexPtr->EntityID = entityID;
		s_Data.LineVertexPtr++;

		s_Data.LineVertexCount += 2;
		s_Data.stats.QuadCount++;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID )
	{
		glm::vec3 p0 = position;
		glm::vec3 p1 = { position.x + size.x, position.y, position.z };
		glm::vec3 p2 = { position.x + size.x, position.y + size.y, position.z };
		glm::vec3 p3 = { position.x, position.y + size.y, position.z };

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);

	}
	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadPosition[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer2D::SetLineWidth(float lineWidth)
	{
		s_Data.LineWidth = lineWidth;
	}
	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}


	void Renderer2D::ResetStats()
	{
		memset(&s_Data.stats, 0, sizeof(RendererStats));
	}

}
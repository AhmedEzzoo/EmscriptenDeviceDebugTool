#pragma once

#include "OrthoGraphicCamera.h"
#include "Camera.h"
#include "Texture.h"

#include "EditorCamera.h"
#include "Scene/Component.h"

namespace Ezzoo {


	class Renderer2D
	{
	public :
		
		static void Init();
		static void ShutDown();


		static void BeginScene(const OrthoGraphicCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void Flush();
		static void FlusAndReset();
		static void StartBatch();



		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);


		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, float tiligFactor, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, float tiligFactor, const glm::vec4& color);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, float tiligFactor, const Ref<Texture2D>& texture);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, float tiligFactor, const Ref<Texture2D>& texture);

		static void DrawQuad(const glm::mat4& transform,  const glm::vec4& color, int entityID = -1);

		static void DrawQuad(const glm::mat4& transform, const SpriteRendererComponent& color, int entityID);
		static void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const glm::vec4& color, float TilingFactor, int entityID);



		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);


		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void SetLineWidth(float lineWidth);
		static float GetLineWidth();


		struct RendererStats
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetIndeciesCount() { return QuadCount * 6; }
			uint32_t GetVertexCount() { return QuadCount * 4; }
		};

		static RendererStats& GetStats();

		static void ResetStats();





	private :

	};
}
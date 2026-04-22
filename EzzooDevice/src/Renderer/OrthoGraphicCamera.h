#pragma once

#include "glm/glm.hpp"

namespace Ezzoo {

	class OrthoGraphicCamera
	{
	public :

		OrthoGraphicCamera(float left, float right, float bottom, float top);


		inline void setProjection(const glm::mat4& projection) {m_ProjectionMatrix = projection; RecalculateViewProjection();}
		void setProjection(float left, float right, float bottom, float top);

		inline const glm::mat4& GetProjection() const  { return m_ProjectionMatrix; }

		inline void setView(const glm::mat4& view) { m_ViewMatrix = view; RecalculateViewProjection(); }
		inline const glm::mat4& GetView() const { return m_ViewMatrix; }

		inline void setViewProjection(const glm::mat4& viewProjection) 
		{
			m_ViewProjectionMatrix = viewProjection; RecalculateViewProjection();
		}
		inline const glm::mat4& GetViewProjection() const {
			return m_ViewProjectionMatrix;
		}

		inline void SetPosition(const glm::vec3& position) {
			m_Position = position; RecalculateViewProjection();
		}
		inline const glm::vec3& GetPosition() const { return m_Position; }

		inline void SetRotation(float rotation) { m_Rotation = rotation;  }
		inline float GetRotation() const { return m_Rotation; }

		inline void SetScale(const glm::vec3& scale) { m_Scale = scale; RecalculateViewProjection(); }
		inline const glm::vec3& GetScale() const { return m_Scale ; }

	private :
		void RecalculateViewProjection();


	private :
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		glm::vec3 m_Scale = {0.0f, 0.0f, 0.0f};

		float m_Rotation = 0.0f;

	};
}
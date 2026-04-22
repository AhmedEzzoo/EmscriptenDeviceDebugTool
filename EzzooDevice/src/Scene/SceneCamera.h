#pragma once

#include "Renderer/Camera.h"

namespace Ezzoo {

	class SceneCamera : public Camera
	{
	public :
		enum class ProjectionType { Prespective = 0, Orthographic = 1 };
		SceneCamera();
		virtual ~SceneCamera() = default;



		void SetOrthogrphic(float size, float nearClip, float farClip);
		void SetPrespective(float fov, float nearClip, float farClip);
		void SetViewportSize(uint32_t  width, uint32_t height);

		void SetProjectionTypeView(const ProjectionType& projection) { m_ProjectionType = projection; RecalculateMatrix(); }
		ProjectionType GetProjectionTypeView() const { return m_ProjectionType; }


		// Orthographic Camera
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size)
		{
			m_OrthographicSize = size;
			RecalculateMatrix();
		}
		float GetOrthographicNear() const { return m_NearClip; }
		void SetOrthographicNear(float nearClip)
		{
			m_NearClip = nearClip;
			RecalculateMatrix();
		}

		float GetOrthographicFar() const { return m_FarClip; }
		void SetOrthographicFar(float farClip)
		{
			m_FarClip = farClip;
			RecalculateMatrix();
		}

		// Prespective Camera
		float GetPrespectiveFov() const { return m_PrespectiveFov; }
		void SetPrespectiveFov(float fov)
		{
			m_PrespectiveFov = fov;
			RecalculateMatrix();
		}
		float GetPrespectiveNear() const { return m_PrespectiveNear; }
		void SetPrespectiveNear(float nearClip)
		{
			m_PrespectiveNear = nearClip;
			RecalculateMatrix();
		}

		float GetPrespectiveFar() const { return m_PrespectiveFar; }
		void SetPrespectiveFar(float farClip)
		{
			m_PrespectiveFar = farClip;
			RecalculateMatrix();
		}

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type)
		{
			m_ProjectionType = type;
			RecalculateMatrix();
		}



	private :
		void RecalculateMatrix();

	private :

		float m_OrthographicSize = 10.0f;
		float m_NearClip = -1.0f, m_FarClip = 1.0f;

		float m_PrespectiveFov = glm::radians(45.0f);
		float m_PrespectiveNear = 0.01f, m_PrespectiveFar = 1000.0f;

		float m_AspectRatio = 0.0f;

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
	};
}
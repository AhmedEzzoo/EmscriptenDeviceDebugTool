#include "ezpch.h"

#include "SceneCamera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Ezzoo {


	SceneCamera::SceneCamera()
	{
		RecalculateMatrix();
	}


	void SceneCamera::SetOrthogrphic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_NearClip = nearClip;
		m_FarClip = farClip;
		RecalculateMatrix();
	}

	void SceneCamera::SetPrespective(float fov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Prespective;
		m_PrespectiveFov = fov;
		m_PrespectiveNear = nearClip;
		m_PrespectiveFar = farClip;

		RecalculateMatrix();
	}

	void SceneCamera::SetViewportSize(uint32_t  width, uint32_t height)
	{
		m_AspectRatio = (width <= 0 || height <= 0 ) ? 9.0f/16.0f : (float)width / (float)height;

		RecalculateMatrix();
	}

	void SceneCamera::RecalculateMatrix()
	{

		if (m_ProjectionType == ProjectionType::Prespective)
		{
			m_Projection = glm::perspective(m_PrespectiveFov, m_AspectRatio, m_PrespectiveNear, m_PrespectiveFar);
		}
		else
		{
			float left = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float right = m_OrthographicSize * m_AspectRatio * 0.5f;
			float bottom = -m_OrthographicSize  * 0.5f;
			float top = m_OrthographicSize * 0.5f;


			m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);

		}
	}



}
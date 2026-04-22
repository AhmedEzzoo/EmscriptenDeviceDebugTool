#include "ezpch.h"

#include "OrthgraphicCameraController.h"

#include "Core/MouseCodes.h"
#include "Core/Input.h"

namespace Ezzoo {


	OrthgraphicCameraController::OrthgraphicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{
		
	}

	void  OrthgraphicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseScrolledEvent>(EZZOO_BIND(OrthgraphicCameraController::OnMouseScrolledEvent));
		dispatcher.Dispatch<WindowResizedEvent>(EZZOO_BIND(OrthgraphicCameraController::OnWindowResizeEvent));
	}

	void OrthgraphicCameraController::OnUpdate(TimeStep ts)
	{

		//Translation
		if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_RIGHT))
			m_CameraPosition.x += m_CameraTranslateSpeed * ts;

		else if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_LEFT))
			m_CameraPosition.x -= m_CameraTranslateSpeed * ts;

		if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_UP))
			m_CameraPosition.y += m_CameraTranslateSpeed * ts;

		else if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_DOWN))
			m_CameraPosition.y -= m_CameraTranslateSpeed * ts;

		//Scale
		if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_S))
			m_CameraScale.x += m_CameraScaleSpeed * ts;

		else if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_W))
			m_CameraScale.x -= m_CameraScaleSpeed * ts;


		//Rotation
		if (m_Rotation)
		{
			if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_A))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			else if (Ezzoo::Input::IsKeyPressed(EZZOO_KEY_D))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			
			m_Camera.SetRotation(m_CameraRotation);
		}



		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetScale(m_CameraScale);

		m_CameraTranslateSpeed = m_ZoomLevel;
	}

	bool OrthgraphicCameraController::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYoffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel,0.25f);

		m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		
		return false;
	}

	bool OrthgraphicCameraController::OnWindowResizeEvent(WindowResizedEvent& e)
	{
		m_AspectRatio = (float)e.GetWindowWidth() / (float)e.GetWindowHeight();
		m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}

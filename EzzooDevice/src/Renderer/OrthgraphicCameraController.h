#pragma once

#include "OrthoGraphicCamera.h"

#include "Core/TimeStep.h"

#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
namespace Ezzoo {

	class OrthgraphicCameraController
	{
	public:
		OrthgraphicCameraController(float aspectRatio, bool rotation = false);
		~OrthgraphicCameraController() = default;

		void OnUpdate(TimeStep ts);

		void OnEvent(Event& e);

		OrthoGraphicCamera& GetCamera() { return m_Camera; }

	private :
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnWindowResizeEvent(WindowResizedEvent& e);

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		float m_CameraScaleSpeed = 1.0f;
		bool m_Rotation;

		glm::vec3 m_CameraScale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };

		OrthoGraphicCamera m_Camera;

		float m_CameraTranslateSpeed = 5.0f;
		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 30.0f;
		
	};
}
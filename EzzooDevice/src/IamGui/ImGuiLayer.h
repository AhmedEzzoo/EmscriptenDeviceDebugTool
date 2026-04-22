#pragma once

#include "Core/Layer.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"
namespace Ezzoo {

	class ImGuiLayer :public Layer
	{

	public:

		ImGuiLayer();
		virtual ~ImGuiLayer();


		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		void SetBlockEvents(bool block) { m_BlockEvents = block; }


		void Begin();
		void End();

	private:
		float m_Time{ 0.0f };

		bool m_BlockEvents = false;

	};
}
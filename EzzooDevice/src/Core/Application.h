#pragma once

#include "Window.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"

#include "LayerStack.h"
#include "IamGui/ImGuiLayer.h"

#include "Renderer/Renderer.h"

#include <filesystem>

//#include <future>

int main(int argc, char** argv);

namespace Ezzoo {

	struct AppCommandLineArgs
	{
		int Count;

		char** Args;

		const char* operator[] (int index)
		{
			//EZZOO_CORE_ASSERT(!(index > 1), "Index Of command line Should be > 1");
			Count = index;

			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name;
		std::filesystem::path path;

		AppCommandLineArgs CommandLineArgs;

		ApplicationSpecification() = default;
	};


	class Application
	{

	public:

		Application(const ApplicationSpecification& specs);
		virtual ~Application();
		void Run();

		
		void OnEvent(Event &event);
		
		void PushLayer(Layer* layer);
		void PushOverLayer(Layer* layer);

		static Application& GetApplication();

		void Close();

		Window& GetWindow();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		//void SubmitMainThreadEvent(const std::function<void()>& func);
		//std::future<bool> m_Future;

		const ApplicationSpecification& GetSpecs() const { return m_Specification; }
		
	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);
		bool OnWindowResizedEvent(WindowResizedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);

		//void ExcuteMainThreadQueue();


	private:
		std::unique_ptr<Window> m_AppWindow;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		LayerStack m_LayerStack;
		bool m_Running{ true };
		bool m_Minimized = false;

		static Application* s_Instance;
		float m_LastMeasuredTime = 0.0f;
		ApplicationSpecification m_Specification;

		std::vector <std::function<void()>> m_MainThreadQueue;
		std::mutex m_LockMutex;

		friend int ::main(int argc, char** argv);
	};

	Application* CreatApplication(AppCommandLineArgs commandLineArgs);

}




#include "ezpch.h"
#include "Core/Input.h"
#include "Core/Application.h"
#include "GLFW/glfw3.h"

namespace Ezzoo {


	bool Input::IsKeyPressed(int keyCode)
	{
		auto status = glfwGetKey(static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow()), keyCode);
		return status;
	}
	bool Input::IsMousePressed(MouseCodes mouseCode)
	{
		auto status =glfwGetMouseButton(static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow()), (int)mouseCode);
		return status;
	}
	std::pair<float, float> Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
		double xPos, yPos;
		
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	}
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();

		return x;
	}
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();

		return y;
	}


}
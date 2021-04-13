#include "Precompiled.h"
#include "Input.h"

#include "Application.h"

#include <GLFW/glfw3.h>


namespace Engine
{
	bool Input::IsKeyPressed(KeyCode key)
	{
		GLFWwindow *window = Application::GetInstance()->GetWindow()->GetWindowPointer();
		auto state = glfwGetKey(window, static_cast<u32>(key));
		return state == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonPressed(MouseCode code)
	{
		GLFWwindow *window = Application::GetInstance()->GetWindow()->GetWindowPointer();
		auto state = glfwGetMouseButton(window, static_cast<u32>(code));
		return state == GLFW_PRESS;
	}
	
	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow *window = Application::GetInstance()->GetWindow()->GetWindowPointer();
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return glm::vec2 { static_cast<float>(xPos), static_cast<float>(yPos) };
	}
}
#include "Precompiled.h"
#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Engine
{
	Window::Window(const std::string &title, u32 width, u32 height)
	{
		int success = glfwInit();
		ME_ASSERT(success);
		ME_INFO("Initialized GLFW");

		ME_INFO("Creating Window %s : %d, %d", title.c_str(), width, height);
		m_WindowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
			title.c_str(), nullptr, nullptr);
		ME_ASSERT(m_WindowHandle);

		glfwMakeContextCurrent(m_WindowHandle);
		success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		ME_ASSERT(success);
		ME_INFO("Initialized Glad");

		SetEventCallbacks();
	}
	Window::~Window()
	{
		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::Maximize()
	{
		glfwMaximizeWindow(m_WindowHandle);
	}

	u32 Window::GetWidth() const
	{
		int width, height;
		glfwGetWindowSize(m_WindowHandle, &width, &height);
		return static_cast<u32>(width);
	}
	u32 Window::GetHeight() const
	{
		int width, height;
		glfwGetWindowSize(m_WindowHandle, &width, &height);
		return static_cast<u32>(height);
	}

	void Window::SetEventCallbacks()
	{
		glfwSetErrorCallback([](int error, const char *description)
		{
			ME_ERROR("%d: %s", error, description);
		});
	}
}
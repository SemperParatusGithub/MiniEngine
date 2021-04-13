#include "Precompiled.h"
#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Event.h"


namespace Engine
{
	Window::Window(const std::string &title, u32 width, u32 height)
	{
		int success = glfwInit();
		ME_ASSERT(success);
		ME_INFO("Initialized GLFW");

		glfwWindowHint(GLFW_SAMPLES, 4);

		ME_INFO("Creating Window %s : %d, %d", title.c_str(), width, height);
		m_WindowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
			title.c_str(), nullptr, nullptr);
		ME_ASSERT(m_WindowHandle);

		glfwMakeContextCurrent(m_WindowHandle);
		success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		ME_ASSERT(success);
		ME_INFO("Initialized Glad");

		glfwSetWindowUserPointer(m_WindowHandle, &m_EventBuffer);

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

	GLFWwindow *Window::GetWindowPointer() const
	{
		return m_WindowHandle;
	}

	std::vector<Event> &Window::GetEventBuffer()
	{
		for (std::size_t i = 0; i < m_EventBuffer.size(); i++)
			if (m_EventBuffer[i].handled)
				m_EventBuffer.erase(m_EventBuffer.begin() + i);

		return m_EventBuffer;
	}
	void Window::ClearEventBuffer()
	{
		m_EventBuffer.clear();
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
		// Error Callback
		glfwSetErrorCallback([](int error, const char *description)
		{
			ME_ERROR("%d: %s", error, description);
		});

		// OpenGL Callback
		glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
			{
				switch (severity)
				{
				case GL_DEBUG_SEVERITY_HIGH:
					ME_ERROR("[OpenGL Debug HIGH] %s", message);
					ME_ASSERT(false);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					ME_WARN("[OpenGL Debug MEDIUM] %s", message);
					break;
				case GL_DEBUG_SEVERITY_LOW:
					ME_WARN("[OpenGL Debug LOW] %s", message);
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					ME_INFO("[OpenGL Debug NOTIFICATION] %s", message);
					break;
				}
			}, nullptr);

		// Window Events
		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow *window)
		{
			auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
			Event event;
			event.type = EventType::WindowClosed;
			eventQueue.push_back(event);
		});
		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow *window, int sizeX, int sizeY)
		{
			auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
			Event event;
			event.type = EventType::WindowResized;
			event.window.size = { static_cast<float>(sizeX), static_cast<float>(sizeY) };
			eventQueue.push_back(event);
		});
		glfwSetWindowPosCallback(m_WindowHandle, [](GLFWwindow *window, int positionX, int positionY)
		{
			auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
			Event event;
			event.type = EventType::WindowMoved;
			event.window.position = { static_cast<float>(positionX), static_cast<float>(positionY) };
			eventQueue.push_back(event);
		});

		// KeyEvents
		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow *window, int key, int scanCode, int action, int mods)
		{
			auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
			Event event;

			switch (action)
			{
				case GLFW_PRESS:
				{
					event.type = EventType::KeyPressed;
					event.key.code = key;
					event.key.repeatCount = 0;
					break;
				}
				case GLFW_RELEASE:
				{
					event.type = EventType::KeyReleased;
					event.key.code = key;
					break;
				}
				case GLFW_REPEAT:
				{
					event.type = EventType::KeyPressed;
					event.key.code = key;
					event.key.repeatCount = 1;
					break;
				}
			}

			eventQueue.push_back(event);
		});
	
		// Mouse Events
		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow *window, int button, int action, int mods)
		{
			auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
			Event event;

			switch (action)
			{
				case GLFW_PRESS:
				{
					event.type = EventType::MouseButtonPressed;
					event.mouse.code = button;
					break;
				}
				case GLFW_RELEASE:
				{
					event.type = EventType::MouseButtonReleased;
					event.mouse.code = button;
					break;
				}
			}

			eventQueue.push_back(event);
		});
		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow *window, double xPosition, double yPosition)
		{
			auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
			Event event;
			event.type = EventType::MouseMoved;
			event.mouse.position = { static_cast<float>(xPosition), static_cast<float>(yPosition) };
			eventQueue.push_back(event);
		});
		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow *window, double xOffset, double yOffset)
			{
				auto &eventQueue = *reinterpret_cast<std::vector<Event>*>(glfwGetWindowUserPointer(window));
				Event event;
				event.type = EventType::MouseScrolled;
				event.mouse.offset = { static_cast<float>(xOffset), static_cast<float>(yOffset) };
				eventQueue.push_back(event);
			});
	}
}
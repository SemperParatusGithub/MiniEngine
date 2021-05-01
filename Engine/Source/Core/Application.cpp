#include "Precompiled.h"
#include "Application.h"

#include "Event.h"

#include "Graphics/Renderer.h"
#include "Graphics/ImGuiHelper.h"

#include <GLFW/glfw3.h>


namespace Engine
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		m_Running = true;

		ME_INFO("Starting up ...");

		m_Window = MakeUnique<Window>("Mini Engine", 1280, 720);

		Renderer::Initialize();
		ImGuiHelper::Initialize();
	}

	Application::~Application()
	{
		Renderer::Shutdown();

		ME_INFO("Shutting down ...");
	}

	void Application::Run()
	{
		OnCreate();

		while (m_Running)
		{
			m_Window->SwapBuffers();
			m_Window->PollEvents();

			float deltaTime = (float) glfwGetTime() - m_LastFrame;
			m_LastFrame = (float) glfwGetTime();

			for (auto &event : m_Window->GetEventBuffer())
			{
				if (event.type == EventType::WindowClosed)
					m_Running = false;

				OnEvent(event);
			}

			m_Window->ClearEventBuffer();

			OnUpdate(deltaTime);

			ImGuiHelper::BeginFrame();
			OnImGui();
			ImGuiHelper::EndFrame();
		}

		OnDestroy();
	}
}
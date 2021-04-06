#include "Precompiled.h"
#include "Application.h"


namespace Engine
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		m_Running = true;

		m_Window = MakeUnique<Window>("Mini Engine", 1280, 720);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		OnCreate();

		while (m_Running)
		{
			m_Window->SwapBuffers();
			m_Window->PollEvents();

			OnUpdate();
			OnImGui();
		}

		OnDestroy();
	}
}
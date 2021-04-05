#include "Precompiled.h"
#include "Application.h"


namespace Engine
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		m_Running = true;
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		OnCreate();

		while (m_Running)
		{
			OnUpdate();
			OnImGui();
		}

		OnDestroy();
	}
}
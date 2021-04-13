#include "Precompiled.h"
#include "Application.h"

#include "Event.h"

#include "Graphics/Renderer.h"
#include "Graphics/ImGuiHelper.h"


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

			for (auto &event : m_Window->GetEventBuffer())
			{
				if (event.type == EventType::WindowClosed)
					m_Running = false;

				// if (event.type == EventType::KeyPressed)
				//	  ME_TRACE("Key Pressed: %d, Repeat Count: %d", event.key.code, event.key.repeatCount);

				OnEvent(event);
			}

			m_Window->ClearEventBuffer();

			OnUpdate(0.0f);

			ImGuiHelper::BeginFrame();
			OnImGui();
			ImGuiHelper::EndFrame();
		}

		OnDestroy();
	}
}
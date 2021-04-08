#pragma once
#include "EngineBase.h"
#include "Window.h"


namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate(float delta) = 0;
		virtual void OnEvent(Event &e) = 0;
		virtual void OnImGui() = 0;

		void Run();

		UniquePtr<Window> &GetWindow() { return m_Window; }

		static Application *GetInstance() { return s_Instance; }

	private:
		static Application *s_Instance;

	private:
		bool m_Running;
		UniquePtr<Window> m_Window;
	};
}
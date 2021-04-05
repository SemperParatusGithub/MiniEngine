#pragma once
#include "EngineBase.h"


namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnImGui() = 0;

		void Run();

		Application *GetInstance() { return s_Instance; }

	private:
		static Application *s_Instance;

		bool m_Running;
	};
}
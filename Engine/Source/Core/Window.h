#pragma once
#include "EngineBase.h"


struct GLFWwindow;

namespace Engine
{
	class Window
	{
	public:
		Window(const std::string &title, u32 width, u32 height);
		~Window();

		void SwapBuffers();
		void PollEvents();

		void Maximize();

		u32 GetWidth() const;
		u32 GetHeight() const;

	private:
		void SetEventCallbacks();

	private:
		GLFWwindow *m_WindowHandle;
	};
}
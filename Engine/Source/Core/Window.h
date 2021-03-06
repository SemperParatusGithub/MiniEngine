#pragma once
#include "EngineBase.h"

#include <vector>
#include <glm/glm.hpp>


struct GLFWwindow;

namespace Engine
{
	struct Event;

	class Window
	{
	public:
		Window(const std::string &title, u32 width, u32 height);
		~Window();

		void SwapBuffers();
		void PollEvents();

		GLFWwindow *GetWindowPointer() const;

		std::vector<Event> &GetEventBuffer();
		void ClearEventBuffer();

		void SetVSync(bool enabled = true);

		void Maximize();

		const glm::vec2& GetPosition() const;

		u32 GetWidth() const;
		u32 GetHeight() const;

	private:
		void SetEventCallbacks();

	private:
		GLFWwindow *m_WindowHandle;
		std::vector<Event> m_EventBuffer;
	};
}
#include "EngineBase.h"

#include <glm/glm.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26495)
#endif


namespace Engine
{
	enum class EventType
	{
		None = 0,

		// Window Events
		WindowClosed, WindowResized, WindowMoved,

		// Key Events
		KeyPressed, KeyReleased,

		// Mouse Events
		MouseButtonPressed, MouseButtonReleased,
		MouseMoved, MouseScrolled
	};

	struct WindowEventData
	{
		glm::vec2 position;		// WindowMoved
		glm::vec2 size;			// WindowResized
	};
	struct KeyEventData
	{
		int code;				// KeyPressed, KeyReleased
		u32 repeatCount;		// KeyPressed
	};
	struct MouseEventData
	{
		int code;				// MouseButtonPressed, MouseButtonReleased
		glm::vec2 position;		// MouseMoved
		glm::vec2 offset;		// MouseScrolled
	};

	struct Event
	{
		EventType type = EventType::None;
		bool handled = false;

		union
		{
			WindowEventData window;
			KeyEventData key;
			MouseEventData mouse;
		};
	};
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
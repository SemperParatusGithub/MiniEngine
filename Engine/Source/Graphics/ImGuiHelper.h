#pragma once
#include "Core/EngineBase.h"


namespace Engine
{
	class ImGuiHelper
	{
	public:
		static void Initialize();

		static void BeginFrame();
		static void EndFrame();

		static void BeginDockspace();
		static void EndDockspace();
	};
}
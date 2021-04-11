#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>

#include "GraphicsPipeline.h"


namespace Engine
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const glm::vec4 &clearColor);

		static void SubmitPipeline(const GraphicsPipeline &pipeline);
	};
}
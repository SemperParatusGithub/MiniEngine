#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>

#include "GraphicsPipeline.h"


namespace Engine
{
	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const glm::vec4 &clearColor);

		static void SubmitQuad(const SharedPtr<Shader> &shader);
		static void SubmitLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec4 color, float thickness = 1.0f);

		static void SubmitPipeline(const GraphicsPipeline &pipeline);
	};
}
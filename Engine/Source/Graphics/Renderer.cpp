#include "Precompiled.h"
#include "Renderer.h"

#include <glad/glad.h>


namespace Engine
{
	void Renderer::Initialize()
	{
		ME_INFO("Initializing Renderer");
	}
	void Renderer::Shutdown()
	{
		ME_INFO("Shutting down Renderer");
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void Renderer::SetClearColor(const glm::vec4 &clearColor)
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void Renderer::SubmitPipeline(const GraphicsPipeline &pipeline)
	{
		pipeline.Bind();
		glDrawElements(GL_TRIANGLES, pipeline.m_IndexBuffer->GetCount(),
			pipeline.m_IndexBuffer->GetType(), nullptr);
	}
}
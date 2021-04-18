#include "Precompiled.h"
#include "Renderer.h"

#include <glad/glad.h>


namespace Engine
{
	struct RendererData
	{
		GraphicsPipeline quadPipeline;
	};
	static RendererData s_RendererData;

	void Renderer::Initialize()
	{
		ME_INFO("Initializing Renderer");

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
		};
		u32 indices[] = {
			0, 1, 2, 2, 3, 0
		};

		Engine::PipelineLayout layout = {
			{ "a_Position", Engine::VertexFormat::Float3, false },
			{ "a_TexCoords", Engine::VertexFormat::Float2, false }
		};

		auto shader = MakeShared<Shader>("Test.glsl");

		auto vertexBuffer = MakeShared<Engine::VertexBuffer>(vertices, sizeof(vertices), Engine::BufferUsage::Static);
		auto indexBuffer = MakeShared<Engine::IndexBuffer>(indices, sizeof(indices), Engine::IndexFormat::Uint32, Engine::BufferUsage::Static);

		s_RendererData.quadPipeline.Create(layout, shader, vertexBuffer, indexBuffer);
	}
	void Renderer::Shutdown()
	{
		ME_INFO("Shutting down Renderer");
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Renderer::SetClearColor(const glm::vec4 &clearColor)
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void Renderer::SubmitQuad(const SharedPtr<Shader> &shader)
	{
		s_RendererData.quadPipeline.m_Shader = shader;
		s_RendererData.quadPipeline.Bind();
		SubmitPipeline(s_RendererData.quadPipeline);
	}

	void Renderer::SubmitLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec4 color, float thickness)
	{
	}

	void Renderer::SubmitPipeline(const GraphicsPipeline &pipeline)
	{
		pipeline.Bind();
		glDrawElements(GL_TRIANGLES, pipeline.m_IndexBuffer->GetCount(),
			pipeline.m_IndexBuffer->GetType(), nullptr);
	}
}
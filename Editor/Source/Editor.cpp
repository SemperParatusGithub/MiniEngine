#include "Editor.h"


Editor::Editor()
{
}
Editor::~Editor()
{
}

void Editor::OnCreate()
{
	float vertices[] = {
	  // Position				Color
		-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f
	};
	u32 indices[] = {
		0, 1, 2
	};

	Engine::PipelineLayout layout = {
		{ "a_Position", Engine::VertexFormat::Float3, false },
		{ "a_Color",    Engine::VertexFormat::Float4, false }
	};

	auto shader = MakeShared<Engine::Shader>("Test.glsl");

	auto vertexBuffer = MakeShared<Engine::VertexBuffer>(vertices, sizeof(vertices), Engine::BufferUsage::Static);
	auto indexBuffer = MakeShared<Engine::IndexBuffer>(indices, sizeof(indices), Engine::IndexFormat::Uint32, Engine::BufferUsage::Static);

	m_Pipeline.Create(layout, shader, vertexBuffer, indexBuffer);
}

void Editor::OnDestroy()
{
}

void Editor::OnUpdate(float delta)
{
	Engine::Renderer::SetClearColor(glm::vec4 { 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();
	Engine::Renderer::SubmitPipeline(m_Pipeline);
}

void Editor::OnEvent(Engine::Event &event)
{
}

void Editor::OnImGui()
{
}
#include "Editor.h"

#include <imgui/imgui.h>


Editor::Editor() : 
	m_Camera(45.0f, 1.778f, 0.1f, 1000.0f)
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

	m_Camera.OnResize(1280, 720);

	auto shader = m_Pipeline.GetShader();
	shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());

	Engine::Renderer::SubmitPipeline(m_Pipeline);
}

void Editor::OnEvent(Engine::Event &event)
{
	m_Camera.OnEvent(event);
}

void Editor::OnImGui()
{
	ImGui::Begin("Test Window");
	ImGui::Text("Hello World");
	ImGui::End();
}
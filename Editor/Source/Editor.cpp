#include "Editor.h"

#include <Glad/glad.h>

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>


Editor::Editor() : 
	m_Camera(90.0f, 1.778f, 0.1f, 1000.0f)
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

	m_GridShader = MakeShared<Engine::Shader>("Grid.glsl");
}

void Editor::OnDestroy()
{
}

void Editor::OnUpdate(float delta)
{
	Engine::Renderer::SetClearColor(glm::vec4 { 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	m_GridShader->Bind();
	m_GridShader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	m_GridShader->SetUniformMatrix4("u_Transform", transform);
	m_GridShader->SetUniformFloat3("u_GridColor", glm::vec3(0.2f));
	m_GridShader->SetUniformFloat("u_Segments", 40.0f);
	Engine::Renderer::SubmitQuad(m_GridShader);

	auto shader = m_Pipeline.GetShader();
	shader->Bind();
	shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	Engine::Renderer::SubmitPipeline(m_Pipeline);
}

void Editor::OnEvent(Engine::Event &event)
{
	m_Camera.OnEvent(event);

	if(event.type == Engine::EventType::WindowResized)
	{
		glViewport(0, 0, (int) event.window.size.x, (int) event.window.size.y);
		m_Camera.OnResize((u32) event.window.size.x, (u32) event.window.size.y);
	}
}

void Editor::OnImGui()
{
	ImGui::Begin("Test Window");
	ImGui::Text("Hello World");
	ImGui::End();
}
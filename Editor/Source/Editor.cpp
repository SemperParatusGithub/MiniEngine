#include "Editor.h"

#include <Glad/glad.h>

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>


Editor::Editor(): 
	m_Camera(Engine::CameraType::Orbit)
{
}
Editor::~Editor()
{
}

void Editor::OnCreate()
{
	m_GridShader = MakeShared<Engine::Shader>("Grid.glsl");
	m_TestMesh = MakeShared<Engine::Mesh>("sponza/sponza.obj");

	auto &window = Application::GetInstance()->GetWindow();
	window->Maximize();
	window->SetVSync(false);
}

void Editor::OnDestroy()
{
}

void Editor::OnUpdate(float delta)
{
	m_Camera.OnUpdate(delta);

	Engine::Renderer::SetClearColor(glm::vec4 { 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	auto &shader = m_TestMesh->GetShader();
	shader->Bind();
	shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	glm::mat4 meshTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
		glm::scale(glm::mat4(1.0f), glm::vec3(0.005f));
	Engine::Renderer::SubmitMesh(m_TestMesh, meshTransform);

	m_GridShader->Bind();
	m_GridShader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	glm::mat4 gridTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	m_GridShader->SetUniformMatrix4("u_Transform", gridTransform);
	m_GridShader->SetUniformFloat3("u_GridColor", glm::vec3(0.4f));
	m_GridShader->SetUniformFloat("u_Segments", 40.0f);
	Engine::Renderer::SubmitQuad(m_GridShader);

	auto *app = Application::GetInstance();
	glViewport(0, 0, (int) app->GetWindow()->GetWidth(), (int) app->GetWindow()->GetHeight());
}

void Editor::OnEvent(Engine::Event &event)
{
	m_Camera.OnEvent(event);
}

void Editor::OnImGui()
{
	OnMeshGui(m_TestMesh);

	static int val = 0;
	static bool fps = false;
	static bool renderLines = false;
	static float lineThickness = 1.0f;

	ImGui::Begin("Debug");
	ImGui::Text("Framerate: %.2f", ImGui::GetIO().Framerate);

	if (ImGui::Checkbox("Render Lines", &renderLines))
		Engine::Renderer::RenderLines(renderLines);
	if (ImGui::SliderFloat("Line Thickness", &lineThickness, 0.1f, 10.0f))
		Engine::Renderer::SetLineThickness(lineThickness);

	if (ImGui::Checkbox("fps", &fps))
		m_Camera.SetCameraType(fps ? Engine::CameraType::FPS : Engine::CameraType::Orbit);

	ImGui::InputInt("ID", &val);
	ImGui::Image((ImTextureID) val, ImVec2(128.0f, 128.0f));
	ImGui::End();
}

void Editor::OnMeshGui(SharedPtr<Engine::Mesh> mesh)
{
	ImGui::Begin("Mesh Info");
	// ImGui::Text("Filepath: %s", mesh->GetFilepath().c_str());
	// 
	// static bool flip = false;
	// if (ImGui::Checkbox("Flip UVs", &flip))
	// 	mesh->SetFlipUVs(flip);

	for (auto &material : mesh->GetMaterials())
	{
		std::string name = material.GetName().empty() ? "Unknown" : material.GetName();
		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::TreePop();
		}
	}

	ImGui::End();
}
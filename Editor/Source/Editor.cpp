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
	m_OutlineShader = MakeShared<Engine::Shader>("Outline.glsl");
	m_TestMesh = MakeShared<Engine::Mesh>("DamagedHelmet/DamagedHelmet.gltf");

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

	if (m_IsMeshSelected)
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Engine::Renderer::SetClearColor(glm::vec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	if (m_IsMeshSelected)
		glStencilMask(0);

	// Render Scene here

	if (m_IsMeshSelected)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilMask(0xff);
	}

	// Render selected mesh
	auto& shader = m_TestMesh->GetShader();
	shader->Bind();
	shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	Engine::Renderer::SubmitMesh(m_TestMesh, glm::mat4(1.0f));

	if (m_IsMeshSelected)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		m_OutlineShader->Bind();
		m_OutlineShader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
		Engine::Renderer::SubmitMeshWithShader(m_TestMesh, glm::scale(glm::mat4(1.0), glm::vec3(1.1f)), m_OutlineShader);

		glStencilMask(0xff);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glEnable(GL_DEPTH_TEST);
	}

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

	if (event.type == Engine::EventType::MouseButtonPressed &&
		event.mouse.code == Engine::Mouse::ButtonLeft)
	{
		ME_INFO("Picking");
		m_IsMeshSelected = false;

		auto mouseRay = CastRay();

		auto& subMeshes = m_TestMesh->GetSubMeshes();
		for (u32 i = 0; i < subMeshes.size(); i++)
		{
			auto& subMesh = subMeshes[i];
			glm::mat4 transform = glm::mat4(1.0f);

			Engine::Ray ray = {
				glm::inverse(transform * subMesh.transform) * glm::vec4(mouseRay.origin, 1.0f),
				glm::inverse(glm::mat3(transform) * glm::mat3(subMesh.transform)) * mouseRay.direction
			};

			const auto& triangleMesh = m_TestMesh->GetTriangleRepresentation();
			for (auto& triangle : triangleMesh)
			{
				if (Engine::Math::RayIntersectsTriangle(ray, triangle))
				{
					m_IsMeshSelected = true;
					ME_INFO("Intersection");
				}
			}
		}
	}
}

void Editor::OnImGui()
{
	OnMeshGui(m_TestMesh);

	static int val = 0;
	static bool fps = false;
	static bool renderLines = false;
	static float lineThickness = 1.0f;

	ImGui::Begin("Debug");
	ImGui::Text("Mesh selected: %d", m_IsMeshSelected);
	ImGui::Text("Framerate: %.2f", ImGui::GetIO().Framerate);

	if (ImGui::Checkbox("Render Lines", &renderLines))
		Engine::Renderer::RenderLines(renderLines);
	if (ImGui::SliderFloat("Line Thickness", &lineThickness, 0.1f, 10.0f))
		Engine::Renderer::SetLineThickness(lineThickness);

	if (ImGui::Checkbox("FPS Camera", &fps))
		m_Camera.SetCameraType(fps ? Engine::CameraType::FPS : Engine::CameraType::Orbit);

	ImGui::InputInt("ID", &val);
	ImGui::Image((ImTextureID) val, ImVec2(128.0f, 128.0f));
	ImGui::End();
}

void Editor::OnMeshGui(SharedPtr<Engine::Mesh> mesh)
{
	ImGui::Begin("Mesh Info");

	ImGui::End();
}
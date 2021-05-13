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

	m_Framebuffer = MakeShared<Engine::Framebuffer>(1280, 720);
	m_Framebuffer->multisampled = false;
	m_Framebuffer->samples = 8;
	m_Framebuffer->attachments = {
		Engine::FramebufferTextureFormat::RGBA8,
		Engine::FramebufferTextureFormat::DEPTH24STENCIL8
	};
	m_Framebuffer->Create();
}

void Editor::OnDestroy()
{
}

void Editor::OnUpdate(float delta)
{
	Engine::Renderer::SetClearColor(glm::vec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	m_Camera.OnUpdate(delta);


	if (m_ViewportSizeChanged)
	{
		m_Camera.OnResize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_Framebuffer->Resize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_ViewportSizeChanged = false;
	}
	m_Framebuffer->Bind();
	if (m_IsMeshSelected)
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Engine::Renderer::SetClearColor(glm::vec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	if (m_IsMeshSelected)
		glStencilMask(0);

	// Render Scene here
	auto& shader2 = m_TestMesh->GetShader();
	shader2->Bind();
	shader2->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	Engine::Renderer::SubmitMesh(m_TestMesh, glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 0.0f)));

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
		Engine::Renderer::SubmitMeshWithShader(m_TestMesh, glm::scale(glm::mat4(1.0), glm::vec3(1.05f)), m_OutlineShader);

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

	m_Framebuffer->UnBind();
}

void Editor::OnEvent(Engine::Event &event)
{
	m_Camera.OnEvent(event);

	if (event.type == Engine::EventType::MouseButtonPressed &&
		event.mouse.code == Engine::Mouse::ButtonLeft)
	{
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
				}
			}
		}
	}
}

void Editor::OnImGui()
{
	static bool renderLines = false;
	static float lineThickness = 1.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();

	m_ViewportPosition = {
		ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
		ImGui::GetWindowPos().y + ImGui::GetCursorPos().y
	};
	glm::vec2 newViewportSize = {
		ImGui::GetWindowSize().x,
		ImGui::GetWindowSize().y
	};
	if (newViewportSize != m_ViewportSize) {
		m_ViewportSize = newViewportSize;
		m_ViewportSizeChanged = true;
	}

	ImGui::Image((ImTextureID)m_Framebuffer->GetColorAttachmentRendererID(), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Begin("Debug");
	ImGui::Text("Mesh selected: %d", m_IsMeshSelected);
	ImGui::Text("Framerate: %.2f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("Frametime: %.2f ms", 1.0f / ImGui::GetIO().Framerate * 1000.0f);

	if (ImGui::Checkbox("Render Lines", &renderLines))
		Engine::Renderer::RenderLines(renderLines);
	if (ImGui::SliderFloat("Line Thickness", &lineThickness, 0.1f, 10.0f))
		Engine::Renderer::SetLineThickness(lineThickness);

	ImGui::End();
}
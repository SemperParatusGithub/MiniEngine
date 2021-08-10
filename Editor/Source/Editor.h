#pragma once
#include <Engine.h>

#include "../imgui/imgui.h"


class Editor : public Engine::Application
{
public:
	Editor();
	~Editor();

	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float delta) override;
	virtual void OnEvent(Engine::Event &event) override;
	virtual void OnImGui() override;

	static Engine::Application *Create()
	{
		return new Editor();
	}

private:
	void MainRenderPass();
	void CompositionRenderPass();

	void BeginDockspace();
	void EndDockspace();

	Engine::Ray CastRay()
	{
		auto mx = ImGui::GetMousePos().x - m_ViewportPosition.x;
		auto my = ImGui::GetMousePos().y - m_ViewportPosition.y;

		float mouseX = (mx / m_ViewportSize.x) * 2.0f - 1.0f;
		float mouseY = ((my / m_ViewportSize.y) * 2.0f - 1.0f) * -1.0f;

		glm::vec4 mouseClipPos = { mouseX, mouseY, -1.0f, 1.0f };

		auto inverseProj = glm::inverse(m_Camera.GetProjectionMatrix());
		auto inverseView = glm::inverse(glm::mat3(m_Camera.GetViewMatrix()));

		glm::vec4 ray = inverseProj * mouseClipPos;
		glm::vec3 rayPos = m_Camera.GetPosition();
		glm::vec3 rayDir = inverseView * glm::vec3(ray);

		return { rayPos, rayDir };
	}

private:
	glm::vec2 m_ViewportPosition = { 0.0f, 0.0f };
	glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
	bool m_ViewportSizeChanged = false;

	bool m_ViewportHovered = false;
	bool m_ViewportFocused = false;

private:
	SharedPtr<Engine::TextureCube> m_RadianceMap;
	SharedPtr<Engine::TextureCube> m_IrradianceMap;
	SharedPtr<Engine::Texture> m_BRDFLUTImage;

	SharedPtr<Engine::Shader> m_SkyboxShader;

	SharedPtr<Engine::Framebuffer> m_MainFramebuffer;
	SharedPtr<Engine::Framebuffer> m_FinalFramebuffer;
	Engine::EditorCamera m_Camera;

	SharedPtr<Engine::Shader> m_GridShader;
	SharedPtr<Engine::Shader> m_OutlineShader;
	SharedPtr<Engine::Shader> m_CompositionShader;

	SharedPtr<Engine::Shader> m_PBRShader;

	SharedPtr<Engine::Mesh> m_TestMesh;
	SharedPtr<Engine::Mesh> m_RoundedCube;
	bool m_IsMeshSelected = false;

	bool m_RenderLines = false;

	bool m_EnableTonemapping = true;
	float m_Exposure = 1.0f;

	RendererID skyboxVAO;

	float m_TextureLod = 0.0f;
};
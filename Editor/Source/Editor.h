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

	static void OnMeshGui(SharedPtr<Engine::Mesh> mesh);

	static Engine::Application *Create()
	{
		return new Editor();
	}

private:
	Engine::Ray CastRay()
	{
		auto& window = Engine::Application::GetWindow();
		auto viewportWidth = window->GetWidth();
		auto viewportHeight = window->GetHeight();
		auto mx = Engine::Input::GetMousePosition().x;
		auto my = Engine::Input::GetMousePosition().y;

		float mouseX = (mx / viewportWidth) * 2.0f - 1.0f;
		float mouseY = ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f;

		glm::vec4 mouseClipPos = { mouseX, mouseY, -1.0f, 1.0f };

		auto inverseProj = glm::inverse(m_Camera.GetProjectionMatrix());
		auto inverseView = glm::inverse(glm::mat3(m_Camera.GetViewMatrix()));

		glm::vec4 ray = inverseProj * mouseClipPos;
		glm::vec3 rayPos = m_Camera.GetPosition();
		glm::vec3 rayDir = inverseView * glm::vec3(ray);

		return { rayPos, rayDir };
	}

private:
	Engine::EditorCamera m_Camera;
	SharedPtr<Engine::Shader> m_GridShader;
	SharedPtr<Engine::Shader> m_OutlineShader;
	SharedPtr<Engine::Mesh> m_TestMesh;
	bool m_IsMeshSelected = false;
};
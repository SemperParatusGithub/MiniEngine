#include "Editor.h"
#define NOMINMAX
#include <Windows.h>

#include <Glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGuizmo/ImGuizmo.h>

#include <stb_image/stb_image.h>

#include <GLFW/glfw3.h>		
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>	


static std::string OpenFileDialog(const char* filter)
{
	OPENFILENAMEA ofn;				// common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Engine::Application::GetInstance()->GetWindow()->GetWindowPointer());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}

	return std::string("");
}
static std::string SaveFileDialog(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	CHAR currentDir[256] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Engine::Application::GetInstance()->GetWindow()->GetWindowPointer());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	if (GetCurrentDirectoryA(256, currentDir))
		ofn.lpstrInitialDir = currentDir;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	// Sets the default extension by extracting it from the filter
	ofn.lpstrDefExt = strchr(filter, '\0') + 1;

	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		return ofn.lpstrFile;
	}

	return std::string("");
}

Editor::Editor(): 
	m_Camera(Engine::CameraType::Orbit)
{
}
Editor::~Editor()
{
}

void Editor::OnCreate()
{
	auto& window = Application::GetInstance()->GetWindow();
	window->Maximize();
	window->SetVSync(false);

	m_MainFramebuffer = MakeShared<Engine::Framebuffer>(1280, 720);
	m_MainFramebuffer->multisampled = true;
	m_MainFramebuffer->samples = 8;
	m_MainFramebuffer->attachments = {
		Engine::FramebufferTextureFormat::RGBA32F,
		Engine::FramebufferTextureFormat::DEPTH24STENCIL8
	};
	m_MainFramebuffer->Create();

	m_FinalFramebuffer = MakeShared<Engine::Framebuffer>(1280, 720);
	m_FinalFramebuffer->multisampled = false;
	m_FinalFramebuffer->attachments = {
		Engine::FramebufferTextureFormat::RGBA8
	};
	m_FinalFramebuffer->Create();

	m_EditorScene = MakeShared<Engine::Scene>();
	m_RuntimeScene = MakeShared<Engine::Scene>();
	m_EditorScene->environment = CreateEnvironment("Assets/Environments/Clouds.hdr");

	// Test Scene
	{
		std::vector<Engine::Transform> staticBodies = {
			Engine::Transform({ 0.0f,  0.0f, 0.0f }, {0.0f, 0.0f,   0.0f}, {20.0f, 1.0f, 5.0f}),	// Ground
			Engine::Transform({-3.0f,  4.0f, 0.0f }, {0.0f, 0.0f, glm::radians(-30.0f)}, {5.0f, 1.0f, 1.0f}),
			Engine::Transform({ 3.0f,  7.0f, 0.0f }, {0.0f, 0.0f,  glm::radians(30.0f)}, {5.0f, 1.0f, 1.0f}),
			Engine::Transform({-3.0f, 10.0f, 0.0f }, {0.0f, 0.0f, glm::radians(-30.0f)}, {5.0f, 1.0f, 1.0f})
		};

		std::vector<Engine::Transform> dynamicBodies = {
			Engine::Transform({ 2.0f, 11.0f, 0.0f }, {0.0f, 0.0f, glm::radians(45.0f)}, {1.0f, 1.0f, 1.0f}),	// Ground
			Engine::Transform({-2.0f, 12.0f, 0.0f }, {0.0f, 0.0f, glm::radians(45.0f)}, {1.0f, 1.0f, 1.0f}),
			Engine::Transform({ 2.0f, 13.0f, 0.0f }, {0.0f, 0.0f, glm::radians(45.0f)}, {1.0f, 1.0f, 1.0f}),
			Engine::Transform({-2.0f, 14.0f, 0.0f }, {0.0f, 0.0f, glm::radians(45.0f)}, {1.0f, 1.0f, 1.0f})
		};

		for (std::size_t i = 0; i < staticBodies.size(); i++)
		{
			auto& transform = staticBodies[i];
			std::string name = "Static Body #" + std::to_string(i);
			auto entity = m_EditorScene->CreateEntity(name);
			entity.Get<Engine::TransformComponent>().transform = transform;
			entity.Add<Engine::MeshComponent>("Assets/Meshes/Cube.fbx");
			entity.Add<Engine::Rigidbody2DComponent>();
			entity.Add<Engine::BoxCollider2DComponent>();
		}

		for (std::size_t i = 0; i < dynamicBodies.size(); i++)
		{
			auto& transform = dynamicBodies[i];
			std::string name = "Dynamic Body #" + std::to_string(i);
			auto entity = m_EditorScene->CreateEntity(name);
			entity.Get<Engine::TransformComponent>().transform = transform;
			entity.Add<Engine::MeshComponent>("Assets/Meshes/Cube.fbx");
			auto& body = entity.Add<Engine::Rigidbody2DComponent>();
			auto& collider = entity.Add<Engine::BoxCollider2DComponent>();
			body.Type = Engine::Rigidbody2DComponent::BodyType::Dynamic;
			collider.Restitution = 0.5f;
		}
	}
}

void Editor::OnDestroy()
{
}

void Editor::OnUpdate(float delta)
{
	switch (m_SceneState)
	{
		case SceneState::Editing:
		{
			// Gizmos
			if (Engine::Input::IsKeyPressed(Engine::Key::LeftControl) && !ImGuizmo::IsUsing())
			{
				if (Engine::Input::IsKeyPressed(Engine::Key::D1))
					m_ImGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				if (Engine::Input::IsKeyPressed(Engine::Key::D2))
					m_ImGuizmoOperation = ImGuizmo::OPERATION::ROTATE;
				if (Engine::Input::IsKeyPressed(Engine::Key::D3))
					m_ImGuizmoOperation = ImGuizmo::OPERATION::SCALE;
				if (Engine::Input::IsKeyPressed(Engine::Key::D0))
					m_ImGuizmoOperation = -1;
			}

			break;
		}

		case SceneState::Playing:
		{
			m_RuntimeScene->OnUpdate(delta);
			break;
		}

		case SceneState::Pausing:
		{

			break;
		}
	}


	m_Camera.OnUpdate(delta);

	if (m_ViewportSizeChanged)
	{
		m_Camera.OnResize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_MainFramebuffer->Resize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_FinalFramebuffer->Resize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_ViewportSizeChanged = false;
	}

	// Render
	{
		Engine::Renderer::SetClearColor(glm::vec4{ 0.7f, 0.7f, 0.7f, 1.0f });
		Engine::Renderer::Clear();

		MainRenderPass();
		CompositionRenderPass();
	}
}

void Editor::OnEvent(Engine::Event &event)
{
	m_Camera.OnEvent(event);

	if (Engine::Input::IsKeyPressed(Engine::Key::LeftControl))
	{
		if (event.type == Engine::EventType::KeyPressed && event.key.code == Engine::Key::D)
		{
			if (m_SelectedEntity)
				m_SelectedEntity = m_EditorScene->DuplicateEntity(m_SelectedEntity);
		}
	}

	if (event.type == Engine::EventType::KeyPressed && event.key.code == Engine::Key::Delete)
	{
		if (m_SelectedEntity)
			m_SelectedEntity.Destroy();
	}

	if (event.type == Engine::EventType::MouseButtonPressed && m_SceneState != SceneState::Playing &&
		event.mouse.code == Engine::Mouse::ButtonLeft && m_ViewportHovered && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
	{
		m_SelectedEntity = Engine::Entity();

		auto mouseRay = CastRay();

		auto view = m_EditorScene->GetRegistry().view<Engine::TransformComponent, Engine::MeshComponent>();
		view.each([&](const entt::entity ent, const Engine::TransformComponent& tc, const Engine::MeshComponent& mc)
			{
				auto entity = Engine::Entity(ent, m_EditorScene.get());
				auto& subMeshes = mc.mesh->GetSubMeshes();

				for (u32 i = 0; i < subMeshes.size(); i++)
				{
					auto& subMesh = subMeshes[i];
					glm::mat4 transform = tc.transform.GetTransform();

					Engine::Ray ray = {
						glm::inverse(transform * subMesh.transform) * glm::vec4(mouseRay.origin, 1.0f),
						glm::inverse(glm::mat3(transform) * glm::mat3(subMesh.transform)) * mouseRay.direction
					};

					bool intersection = false;
					const auto& triangleMesh = mc.mesh->GetTriangleRepresentation();

					for (auto& triangle : triangleMesh)
					{
						float distance; 
						if (Engine::Math::RayIntersectsTriangle(ray, triangle, distance))
						{
							m_SelectedEntity = Engine::Entity(ent, m_EditorScene.get());
						}
					}
				}
			});
	}
}

void Editor::OnImGui()
{
	BeginDockspace();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New", "Ctrl+N");
			ImGui::MenuItem("Open...", "Ctrl+O");
			ImGui::MenuItem("Save As...", "Ctrl+Shift+S");
			ImGui::MenuItem("Exit");

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::MenuItem("Option 1");
			ImGui::MenuItem("Option 2");
			ImGui::MenuItem("Option 3");
			ImGui::MenuItem("Option 4");

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::SetNextWindowSizeConstraints(ImVec2(600.0f, 400.0f), ImVec2(4000.0f, 3000.0f));
	ImGui::Begin("Viewport");

	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();

	m_ViewportPosition = {
		ImGui::GetWindowPos().x + ImGui::GetCursorPos().x,
		ImGui::GetWindowPos().y + ImGui::GetCursorPos().y
	};
	glm::vec2 newViewportSize = {
		ImGui::GetWindowSize().x - ImGui::GetCursorPos().x,
		ImGui::GetWindowSize().y - ImGui::GetCursorPos().y
	};
	if (newViewportSize != m_ViewportSize) {
		m_ViewportSize = newViewportSize;
		m_ViewportSizeChanged = true;
	}

	ImGui::Image((ImTextureID) m_FinalFramebuffer->GetColorAttachmentRendererID(), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	UpdateGizmos();

	ImGui::End();	
	ImGui::PopStyleVar();

	DrawDebugInfo();
	DrawEnvironmentSettings();

	DrawHierarchy();
	DrawInspector();

	EndDockspace();
}

void Editor::MainRenderPass()
{
	m_MainFramebuffer->Bind();

	bool isMeshSelected = false;
	if (m_SelectedEntity)
		isMeshSelected = m_SelectedEntity.Has<Engine::MeshComponent>();

	if (isMeshSelected)
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Engine::Renderer::SetClearColor(glm::vec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	if (isMeshSelected)
		glStencilMask(0);


	auto& scene = m_SceneState == SceneState::Playing ? m_RuntimeScene : m_EditorScene;
	// Render Scene
	{
		auto& shader = Engine::Renderer::GetShader("PBR");
		shader->Bind();
		shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
		shader->SetUniformFloat3("u_CameraPosition", m_Camera.GetPosition());

		auto &directionalLight = scene->environment.directionalLight;
		shader->SetUniformInt("u_DirectionalLights[0].Active", directionalLight.active);
		shader->SetUniformFloat3("u_DirectionalLights[0].Direction", directionalLight.direction);
		shader->SetUniformFloat3("u_DirectionalLights[0].Radiance", directionalLight.radiance);
		shader->SetUniformFloat("u_DirectionalLights[0].Multiplier", directionalLight.intensity);

		shader->SetUniformInt("u_BRDFLUTTexture", 5);
		shader->SetUniformInt("u_EnvRadianceTex", 6);
		shader->SetUniformInt("u_EnvIrradianceTex", 7);

		scene->environment.brdflutTexture->Bind(5);
		scene->environment.radianceMap->Bind(6);
		scene->environment.irradianceMap->Bind(7);

		auto view = scene->GetRegistry().view<Engine::TransformComponent, Engine::MeshComponent>();
		view.each([=](const entt::entity ent, const Engine::TransformComponent& tc, const Engine::MeshComponent& mc)
			{
				if (mc.mesh->IsLoaded() && m_SelectedEntity.GetEntity() != ent)
					Engine::Renderer::SubmitMesh(mc.mesh, tc.transform.GetTransform());
			});
	}

	// Render Skybox
	{
		const auto &shader = Engine::Renderer::GetShader("Skybox");
		const auto &proj = m_Camera.GetProjectionMatrix();
		auto view = glm::mat4(glm::mat3(m_Camera.GetViewMatrix()));

		shader->Bind();	
		shader->SetUniformMatrix4("u_Projection", proj);
		shader->SetUniformMatrix4("u_View", view);
		shader->SetUniformInt("u_ImageCube", 0);
		shader->SetUniformFloat("u_TextureLod", scene->environment.textureLod);
		shader->SetUniformFloat("u_Exposure", scene->environment.exposure);
		
		scene->environment.radianceMap->Bind(0);

		Engine::Renderer::SubmitSkybox(scene->environment.radianceMap, shader);
	}

	// Render selected mesh
	if(isMeshSelected)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilMask(0xff);

		auto &selectedMesh = m_SelectedEntity.Get<Engine::MeshComponent>();
		const auto &transform = m_SelectedEntity.Get<Engine::TransformComponent>();

		if (selectedMesh.mesh->IsLoaded())
		{
			auto& shader = Engine::Renderer::GetShader("PBR");
			shader->Bind();
			shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
			shader->SetUniformFloat3("u_CameraPosition", m_Camera.GetPosition());

			shader->SetUniformFloat3("u_AlbedoColor", glm::vec3(0.8f, 0.1f, 0.15f));

			shader->SetUniformInt("u_EnableAlbedoTexture", 0);
			shader->SetUniformInt("u_EnableNormalMapTexture", 0);
			shader->SetUniformInt("u_EnableMetalnessTexture", 0);
			shader->SetUniformInt("u_EnableRoughnessTexture", 0);

			shader->SetUniformInt("u_DirectionalLights[0].Active", 1);
			shader->SetUniformFloat3("u_DirectionalLights[0].Direction", glm::vec3(glm::radians(30.0f), glm::radians(20.0f), 0.0f));
			shader->SetUniformFloat3("u_DirectionalLights[0].Radiance", glm::vec3(1.0f));
			shader->SetUniformFloat("u_DirectionalLights[0].Multiplier", 1.0f);

			shader->SetUniformInt("u_DirectionalLights[1].Active", 0);
			shader->SetUniformInt("u_DirectionalLights[2].Active", 0);
			shader->SetUniformInt("u_DirectionalLights[3].Active", 0);

			shader->SetUniformInt("u_BRDFLUTTexture", 5);
			shader->SetUniformInt("u_EnvRadianceTex", 6);
			shader->SetUniformInt("u_EnvIrradianceTex", 7);

			scene->environment.brdflutTexture->Bind(5);
			scene->environment.radianceMap->Bind(6);
			scene->environment.irradianceMap->Bind(7);
			Engine::Renderer::SubmitMesh(selectedMesh.mesh, transform.transform.GetTransform());

			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			glLineWidth(10);
			glEnable(GL_LINE_SMOOTH);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			auto& outlineShader = Engine::Renderer::GetShader("Outline");
			outlineShader->Bind();
			outlineShader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
			Engine::Renderer::SubmitMeshWithShader(selectedMesh.mesh, transform.transform.GetTransform() * glm::scale(glm::mat4(1.0f), glm::vec3(1.015f)), outlineShader);

			glPointSize(10);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

			Engine::Renderer::SubmitMeshWithShader(selectedMesh.mesh, transform.transform.GetTransform() * glm::scale(glm::mat4(1.0f), glm::vec3(1.015f)), outlineShader);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glStencilMask(0xff);
			glStencilFunc(GL_ALWAYS, 1, 0xff);
			glEnable(GL_DEPTH_TEST);
		}
	}

	m_MainFramebuffer->UnBind();
}

void Editor::CompositionRenderPass()
{
	m_FinalFramebuffer->Bind();

	auto& shader = Engine::Renderer::GetShader("Composition");
	shader->Bind();
	
	shader->SetUniformInt("u_Texture", 0);
	shader->SetUniformInt("u_TextureSamples", m_MainFramebuffer->samples);
	shader->SetUniformInt("u_EnableTonemapping", m_EnableTonemapping);
	shader->SetUniformFloat("u_Exposure", m_Exposure);
	glBindTextureUnit(0, m_MainFramebuffer->GetColorAttachmentRendererID());
	
	Engine::Renderer::Clear();
	Engine::Renderer::SubmitQuad(shader);
	
	m_FinalFramebuffer->UnBind();
}

Engine::Environment Editor::CreateEnvironment(const std::string& filepath)
{
	Engine::Environment environment;

	constexpr uint32_t cubemapSize = 1024;
	constexpr uint32_t irradianceMapSize = 32;

	SharedPtr<Engine::ComputeShader> EquirectangularToCubemapShader = MakeShared<Engine::ComputeShader>("Assets/Shaders/EquirectangularToCubemap.compute.glsl");

	SharedPtr<Engine::TextureCube> environmentTextureCube = MakeShared<Engine::TextureCube>(cubemapSize, cubemapSize);
	SharedPtr<Engine::Texture> HDRTexture = MakeShared<Engine::Texture>(filepath.c_str());

	EquirectangularToCubemapShader->Bind();

	HDRTexture->Bind(1);
	glBindImageTexture(0, environmentTextureCube->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	ME_INFO("Dispatching compute");

	glDispatchCompute(32, 32, 6);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glGenerateTextureMipmap(environmentTextureCube->GetRendererID());

	ME_INFO("Computation finished");

	// TODO: Filter environment map and create irradiance map for ibl
	SharedPtr<Engine::ComputeShader> environmentFilteringShader = MakeShared<Engine::ComputeShader>("Assets/Shaders/EnvironmentFiltering.compute.glsl");
	SharedPtr<Engine::TextureCube> filteredEnvironmentTextureCube = MakeShared<Engine::TextureCube>(cubemapSize, cubemapSize);

	glCopyImageSubData(environmentTextureCube->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
		filteredEnvironmentTextureCube->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
		filteredEnvironmentTextureCube->GetWidth(), filteredEnvironmentTextureCube->GetHeight(), 6);

	environmentFilteringShader->Bind();
	environmentTextureCube->Bind(1);

	const float deltaRoughness = 1.0f / glm::max(float(filteredEnvironmentTextureCube->GetMipLevelCount()) - 1.0f, 1.0f);
	for (uint32_t level = 1, size = cubemapSize / 2; level < filteredEnvironmentTextureCube->GetMipLevelCount(); level++, size /= 2) // <= ?
	{
		glBindImageTexture(0, filteredEnvironmentTextureCube->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		const GLint roughnessUniformLocation = glGetUniformLocation(environmentFilteringShader->GetRendererID(), "u_Roughness");
		ME_ASSERT(roughnessUniformLocation != -1);
		glUniform1f(roughnessUniformLocation, (float)level * deltaRoughness);

		const GLuint numGroups = glm::max(1u, size / 32);
		glDispatchCompute(numGroups, numGroups, 6);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}


	SharedPtr<Engine::ComputeShader> envIrradianceShader = MakeShared<Engine::ComputeShader>("Assets/Shaders/EnvironmentIrradiance.compute.glsl");

	SharedPtr<Engine::TextureCube> irradianceMap = MakeShared<Engine::TextureCube>(irradianceMapSize, irradianceMapSize);
	envIrradianceShader->Bind();
	filteredEnvironmentTextureCube->Bind(1);

	glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glGenerateTextureMipmap(irradianceMap->GetRendererID());

	environment.radianceMap = filteredEnvironmentTextureCube;
	environment.irradianceMap = irradianceMap;
	environment.brdflutTexture = MakeShared<Engine::Texture>("Assets/Textures/BRDF.tga");
	environment.exposure = 1.0f;
	environment.textureLod = 0.0f;

	environment.directionalLight.active = true;

	return environment;
}

void Editor::BeginDockspace()
{
	ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", 0, dockSpaceWindowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceID = ImGui::GetID("DockSpace");

	// Dockspace
	ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);
}

void Editor::EndDockspace()
{
	ImGui::End();
}

void Editor::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Entity"))
			{
				m_SelectedEntity = m_EditorScene->CreateEntity();
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	auto view = m_EditorScene->GetRegistry().view<Engine::IDComponent>();

	for (auto entity : view)
	{
		Engine::Entity currentEntity = Engine::Entity(entity, m_EditorScene.get());
		bool isActive = currentEntity.GetEntity() == m_SelectedEntity.GetEntity();

		auto& idc = view.get<Engine::IDComponent>(entity);

		ImGui::PushID((int)currentEntity.GetEntity());
		if (ImGui::Selectable(idc.name.c_str(), &isActive))
			m_SelectedEntity = currentEntity;
		ImGui::PopID();
	}

	ImGui::End();
}

void Editor::DrawInspector()
{
	ImGui::Begin("Inspector");

	if (!m_SelectedEntity) {
		ImGui::End();
		return;
	}
	Engine::Entity entity = m_SelectedEntity;

	// ID Component
	{
		auto& idc = entity.Get<Engine::IDComponent>();

		char buffer[64];
		std::memset(buffer, 0, 64);
		std::memcpy(buffer, idc.name.c_str(), idc.name.length());

		float buttonSize = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		float nameWidth = ImGui::GetContentRegionAvail().x - buttonSize;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 5.0f));
		ImGui::SetNextItemWidth(nameWidth);
		if (ImGui::InputText("##Name", buffer, 64))
			idc.name = std::string(buffer);

		ImGui::SameLine();
		if (ImGui::Button("...", ImVec2(buttonSize, buttonSize)))
			ImGui::OpenPopup("Settings");

		ImGui::PopStyleVar();	// Item spacing

		if (ImGui::BeginPopup("Settings"))
		{
			if (ImGui::BeginMenu("Add Component"))
			{
				if (ImGui::MenuItem("Transform"))
					if (!entity.Has<Engine::TransformComponent>())
						entity.Add<Engine::TransformComponent>(Engine::TransformComponent{});


				if (ImGui::MenuItem("Mesh"))
					if (!entity.Has<Engine::MeshComponent>())
						entity.Add<Engine::MeshComponent>(Engine::MeshComponent{});

				if (ImGui::MenuItem("Camera"))
					if (!entity.Has<Engine::CameraComponent>())
						entity.Add<Engine::CameraComponent>(Engine::CameraComponent{});

				if (ImGui::MenuItem("Rigid Body 2D"))
					if (!entity.Has<Engine::Rigidbody2DComponent>())
						entity.Add<Engine::Rigidbody2DComponent>(Engine::Rigidbody2DComponent{});

				if (ImGui::MenuItem("Box Collider 2D"))
					if (!entity.Has<Engine::BoxCollider2DComponent>())
						entity.Add<Engine::BoxCollider2DComponent>(Engine::BoxCollider2DComponent{});

				if (ImGui::MenuItem(" Circle Collider 2D"))
					if (!entity.Has<Engine::CircleCollider2DComponent>())
						entity.Add<Engine::CircleCollider2DComponent>(Engine::CircleCollider2DComponent{});

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Delete Entity"))
			{
				entity.Destroy();
				ImGui::EndPopup();
				ImGui::End();
				return;
			}

			ImGui::EndPopup();
		}
	}

	// Transform Component
	if (entity.Has<Engine::TransformComponent>())
	{
		auto& transform = entity.Get<Engine::TransformComponent>().transform;
		auto translation = entity.Get<Engine::TransformComponent>().transform.GetTranslation();
		auto rotation = entity.Get<Engine::TransformComponent>().transform.GetRotation();
		auto scale = entity.Get<Engine::TransformComponent>().transform.GetScale();

		if (ImGui::CollapsingHeader("Transform Settings"))
		{
			if (ImGui::IsItemHovered() && ImGui::IsMouseDown(1))
				ImGui::OpenPopup("Component Options##Transform");

			glm::vec3 rotationDeg = glm::degrees(rotation);

			ImGui::PushID((int)entity.GetEntity());
			if (DrawSliderFloat3(" Translation", 110.0f, translation, 0.0f))
				transform.SetTranslation(translation);
			if (DrawSliderFloat3(" Rotation", 110.0f, rotationDeg, 0.0f))
				transform.SetRotation(glm::radians(rotationDeg));
			if (DrawSliderFloat3(" Scale", 110.0f, scale, 1.0f))
				transform.SetScale(scale);
			ImGui::PopID();
		}
		else if (ImGui::IsItemHovered() && ImGui::IsMouseDown(1))
			ImGui::OpenPopup("Component Options##Transform");

		if (ImGui::BeginPopup("Component Options##Transform"))
		{
			if (ImGui::MenuItem("Remove Component"))
				entity.Remove<Engine::TransformComponent>();

			ImGui::EndPopup();
		}
	}

	// Mesh Component
	if (entity.Has<Engine::MeshComponent>())
	{
		auto& mc = entity.Get<Engine::MeshComponent>();

		if (ImGui::CollapsingHeader("Mesh Settings"))
		{
			ImGui::TextWrapped("Filepath: %s", mc.mesh->GetFilepath().c_str());
			if (ImGui::Button("Load"))
			{
				std::string filepath = OpenFileDialog("");
				mc.mesh.reset(new Engine::Mesh(filepath));
			}

			static bool showMaterials = false;
			if (ImGui::Button("Open Material Settings"))
				showMaterials = true;

			if (showMaterials && !mc.mesh->GetSubMeshes().empty())
			{
				auto& subMaterials = mc.mesh->GetMaterials();

				ImGui::Begin("Materials", &showMaterials, ImGuiWindowFlags_NoDocking);

				static u32 selected = 0;

				ImGui::BeginChild("left pane", ImVec2(150, 0), true);
				for (int i = 0; i < subMaterials.size(); i++)
				{
					std::string name = subMaterials[i].GetName();
					if (ImGui::Selectable(name.c_str(), i == selected))
						selected = i;
				}
				ImGui::EndChild();
				ImGui::SameLine();

				// If the Mesh changed make sure we don't go out of bounds
				selected = selected >= subMaterials.size() ? 0 : selected;

				ImGui::BeginChild("Settings");
				if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
				{
					if (ImGui::BeginTabItem("Parameters"))
					{
						auto& params = subMaterials[selected].GetParameters();
						ImGui::ColorEdit3("Albedo Color", &params.albedo[0]);
						ImGui::SliderFloat("Metalness", &params.metalness, 0.0f, 1.0f);
						ImGui::SliderFloat("Roughness", &params.roughness, 0.0f, 1.0f);
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Textures"))
					{
						auto& textures = subMaterials[selected].GetTextures();

						auto CheckForClickAndSetTexture = [](SharedPtr<Engine::Texture>& texture)
						{
							if (ImGui::IsItemClicked())
							{
								std::string filepath = OpenFileDialog("");
								if (filepath != "")
									texture = MakeShared<Engine::Texture>(filepath);
							}
						};

						// Albedo
						{
							ImGui::Text("Albedo Texture");
							if (!textures.albedo->IsLoaded())
								ImGui::Image(0, ImVec2(64.0f, 64.0f));
							else
								ImGui::Image((ImTextureID)textures.albedo->GetRendererID(), ImVec2(64.0f, 64.0f));

							CheckForClickAndSetTexture(textures.albedo);
							ImGui::Checkbox("Enable##Albedo", &textures.useAlbedo);
						}

						// Normal Map
						{
							ImGui::Text("Normal Map");
							if (!textures.normal->IsLoaded())
								ImGui::Image(0, ImVec2(64.0f, 64.0f));
							else
								ImGui::Image((ImTextureID)textures.normal->GetRendererID(), ImVec2(64.0f, 64.0f));

							CheckForClickAndSetTexture(textures.normal);
							ImGui::Checkbox("Enable##Normal", &textures.useNormal);
						}

						// Metalness
						{
							ImGui::Text("Metalness Texture");
							if (!textures.metalness->IsLoaded())
								ImGui::Image(0, ImVec2(64.0f, 64.0f));
							else
								ImGui::Image((ImTextureID)textures.metalness->GetRendererID(), ImVec2(64.0f, 64.0f));

							CheckForClickAndSetTexture(textures.metalness);
							ImGui::Checkbox("Enable##Metalness", &textures.useMetalness);
						}

						// Roughness
						{
							ImGui::Text("Roughness Texture");
							if (!textures.roughness->IsLoaded())
								ImGui::Image(0, ImVec2(64.0f, 64.0f));
							else
								ImGui::Image((ImTextureID)textures.roughness->GetRendererID(), ImVec2(64.0f, 64.0f));

							CheckForClickAndSetTexture(textures.roughness);
							ImGui::Checkbox("Enable##Roughness", &textures.useRoughness);
						}

						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::EndChild();
				ImGui::End();
			}
		}
	}

	// Scene Camera
	if (entity.Has<Engine::CameraComponent>())
	{
		auto& camera = entity.Get<Engine::CameraComponent>().camera;

		if (ImGui::CollapsingHeader("Camera Settings"))
		{
			ImGui::Text("Perspective Camera");

			float FOV = camera.GetFOV();
			if (ImGui::SliderFloat("FOV", &FOV, 40.0f, 100.0f))
				camera.SetFOV(FOV);

			float nearClip = camera.GetNearClip();
			float farClip = camera.GetFarClip();

			if (ImGui::SliderFloat("Near Clip", &nearClip, 0.0f, 1000.0f))
				camera.SetNearClip(nearClip);
			if (ImGui::SliderFloat("Far Clip", &farClip, 0.0f, 1000.0f))
				camera.SetFarClip(farClip);
		}
	}

	// BoxCollider 2D
	if (entity.Has<Engine::BoxCollider2DComponent>())
	{
		auto& collider = entity.Get<Engine::BoxCollider2DComponent>();

		if (ImGui::CollapsingHeader("Box Collider 2D Settings"))
		{
			ImGui::SliderFloat2("Size", &collider.Size[0], 0.0f, 1000.0f);
			ImGui::SliderFloat2("Offset", &collider.Offset[0], 0.0f, 1000.0f);

			ImGui::SliderFloat("Density", &collider.Density, 0.0f, 10.0f);
			ImGui::SliderFloat("Friction", &collider.Friction, 0.0f, 10.0f);
			ImGui::SliderFloat("Restitution", &collider.Restitution, 0.0f, 10.0f);
			ImGui::SliderFloat("RestitutionThreshold", &collider.RestitutionThreshold, 0.0f, 10.0f);
		}
	}

	// BoxCollider 2D
	if (entity.Has<Engine::CircleCollider2DComponent>())
	{
		auto& collider = entity.Get<Engine::CircleCollider2DComponent>();

		if (ImGui::CollapsingHeader("Circle Collider 2D Settings"))
		{
			ImGui::SliderFloat("Radius", &collider.Radius, 0.0f, 1000.0f);
			ImGui::SliderFloat2("Offset", &collider.Offset[0], 0.0f, 1000.0f);

			ImGui::SliderFloat("Density", &collider.Density, 0.0f, 10.0f);
			ImGui::SliderFloat("Friction", &collider.Friction, 0.0f, 10.0f);
			ImGui::SliderFloat("Restitution", &collider.Restitution, 0.0f, 10.0f);
			ImGui::SliderFloat("RestitutionThreshold", &collider.RestitutionThreshold, 0.0f, 10.0f);
		}
	}

	// Rigid Body 2D
	if (entity.Has<Engine::Rigidbody2DComponent>())
	{
		auto& body = entity.Get<Engine::Rigidbody2DComponent>();

		if (ImGui::CollapsingHeader("Rigid Body 2D Settings"))
		{
			ImGui::SliderInt("BodyType", (int*)&body.Type, 0, 2);
			switch (body.Type)
			{
				case Engine::Rigidbody2DComponent::BodyType::Static:    ImGui::Text("BodyType::Static");	break;
				case Engine::Rigidbody2DComponent::BodyType::Dynamic:   ImGui::Text("BodyType::Dynamic");	break;
				case Engine::Rigidbody2DComponent::BodyType::Kinematic: ImGui::Text("BodyType::Kinematic");	break;
			}
		}
	}

	ImGui::End();
}

void Editor::DrawDebugInfo()
{
	ImGui::Begin("Debug");
	ImGui::Text("Framerate: %.2f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("Frametime: %.2f ms", 1.0f / ImGui::GetIO().Framerate * 1000.0f);

	static bool renderLines = false;
	if (ImGui::Checkbox("Render Lines", &renderLines))
	{
		Engine::Renderer::SetLineThickness(0.1f);
		Engine::Renderer::RenderLines(renderLines);
	}


	ImGui::Text("Multisampled Anti-Aliasing activated");
	static int samples = 4;
	if (ImGui::SliderInt("Samples", &samples, 2, 16))
	{
		// Recreate Framebuffer
		m_MainFramebuffer->samples = samples;
		m_MainFramebuffer->Create();
	}

	ImGui::Separator();

	ImGui::Checkbox("Tonemapping", &m_EnableTonemapping);
	ImGui::SliderFloat("Exposure", &m_Exposure, 0.1f, 10.0f);

	static bool fpscam = false;
	if (ImGui::Checkbox("FPS Camera", &fpscam))
		m_Camera.SetCameraType(fpscam ? Engine::CameraType::FPS : Engine::CameraType::Orbit);

	if (m_SelectedEntity)
	{
		auto name = m_SelectedEntity.Get<Engine::IDComponent>().name;
		ImGui::Text("Selected entity: %s", name.c_str());
	}

	if (ImGui::Button("Play"))
	{
		m_SceneState = SceneState::Playing;

		if (m_SceneState == SceneState::Pausing)
			return;

		m_SelectedEntity = Engine::Entity();
		m_ImGuizmoOperation = -1;

		Engine::Scene::Copy(m_EditorScene, m_RuntimeScene);
		m_RuntimeScene->SetupPhysicsSimulation();
	}

	if (ImGui::Button("Pause"))
	{
		if (m_SceneState == SceneState::Playing)	// Can't pause when we are not playing
		{
			m_SceneState = SceneState::Pausing;

		}
	}

	if (ImGui::Button("Reset"))
	{
		m_SceneState = SceneState::Editing;

	}

	ImGui::SliderFloat("Physics Simulation Speed", &m_SimulationSpeed, 0.0f, 2.0f);

	ImGui::End();
}

void Editor::DrawEnvironmentSettings()
{
	auto& env = m_EditorScene->environment;
	auto& dl = env.directionalLight;

	ImGui::Begin("Environment");
	ImGui::SliderFloat("Exposure", &env.exposure, 0.1f, 10.0f);
	ImGui::SliderFloat("TextureLod", &env.textureLod, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::Text("Directional Light");
	ImGui::Checkbox("Active", &dl.active);
	DrawSliderFloat3("Direction", 110.0f, dl.direction, 0.0f);
	DrawSliderFloat3("Radiance", 110.0f, dl.radiance, 0.0f);
	ImGui::SliderFloat("Intensity", &dl.intensity, 0.1f, 10.0f);
	ImGui::End();
}

void Editor::UpdateGizmos()
{
	Engine::Entity activeEntity = m_SelectedEntity;
	if (activeEntity && m_ImGuizmoOperation != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_ViewportPosition.x, m_ViewportPosition.y, m_ViewportSize.x, m_ViewportSize.y);
		const glm::mat4 &view = m_Camera.GetViewMatrix();
		const glm::mat4 &proj = m_Camera.GetProjectionMatrix();

		auto& tc = activeEntity.Get<Engine::TransformComponent>();
		glm::mat4 transform = tc.transform.GetTransform();

		bool snap = Engine::Input::IsKeyPressed(Engine::Key::LeftControl);
		float snapValue = m_ImGuizmoOperation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;

		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(&view[0][0], &proj[0][0], (ImGuizmo::OPERATION)m_ImGuizmoOperation,
			ImGuizmo::LOCAL, &transform[0][0], nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			auto [translation, rotation, scale] = Engine::Math::Decompose(transform);

			glm::vec3 deltaRotation = rotation - tc.transform.GetRotation();

			if (m_ImGuizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
				tc.transform.SetTranslation(translation);
			if (m_ImGuizmoOperation == ImGuizmo::OPERATION::ROTATE)
				tc.transform.Rotate(deltaRotation);
			if (m_ImGuizmoOperation == ImGuizmo::OPERATION::SCALE)
				tc.transform.SetScale(scale);
		}
	}
}

bool Editor::DrawSliderFloat3(const std::string &name, float labelWidth, glm::vec3& vector, float resetValue)
{
	bool valuesChanged = false;

	float regionWidth = ImGui::GetContentRegionAvail().x - labelWidth;
	float sz = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

	ImVec2 buttonSize = { sz, sz };
	float sliderSize = regionWidth / 3.0f - buttonSize.x;

	ImGui::PushID(name.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 5.0f });
	ImGui::Text(name.c_str());
	ImGui::SameLine(labelWidth);

	ImGui::SetNextItemWidth(sliderSize);
	if (ImGui::DragFloat("##x", &vector[0], 0.1f, 0.0f, 0.0f, "%.3f"))
		valuesChanged = true;

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize)) {
		vector.x = resetValue;
		valuesChanged = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(sliderSize);
	if (ImGui::DragFloat("##y", &vector[1], 0.1f, 0.0f, 0.0f, "%.3f"))
		valuesChanged = true;

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize)) {
		vector.y = resetValue;
		valuesChanged = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(sliderSize);
	if (ImGui::DragFloat("##z", &vector[2], 0.1f, 0.0f, 0.0f, "%.3f"))
		valuesChanged = true;

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize)) {
		vector.z = resetValue;
		valuesChanged = true;
	}
	ImGui::PopStyleColor(3);

	ImGui::PopStyleVar(2);	// Item Spacing, Frame Rounding

	ImGui::PopID();

	return valuesChanged;
}
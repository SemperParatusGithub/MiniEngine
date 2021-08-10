#include "Editor.h"

#include <Glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image/stb_image.h>

#include <GLFW/glfw3.h>


unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
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
	m_GridShader = MakeShared<Engine::Shader>("Assets/Shaders/Grid.glsl");
	m_OutlineShader = MakeShared<Engine::Shader>("Assets/Shaders/Outline.glsl");
	m_CompositionShader = MakeShared<Engine::Shader>("Assets/Shaders/Composition.glsl");

	m_PBRShader = MakeShared<Engine::Shader>("Assets/Shaders/PBR.glsl");

	//m_TestMesh = MakeShared<Engine::Mesh>("DamagedHelmet/DamagedHelmet.gltf");
	m_TestMesh = MakeShared<Engine::Mesh>("Assets/Meshes/Sphere.fbx");

	auto &window = Application::GetInstance()->GetWindow();
	window->Maximize();
	window->SetVSync(false);

	m_MainFramebuffer = MakeShared<Engine::Framebuffer>(1280, 720);
	m_MainFramebuffer->multisampled = true;
	m_MainFramebuffer->samples = 8;
	m_MainFramebuffer->attachments = {
		Engine::FramebufferTextureFormat::RGBA8,
		Engine::FramebufferTextureFormat::DEPTH24STENCIL8
	};
	m_MainFramebuffer->Create();

	m_FinalFramebuffer = MakeShared<Engine::Framebuffer>(1280, 720);
	m_FinalFramebuffer->multisampled = false;
	m_FinalFramebuffer->attachments = {
		Engine::FramebufferTextureFormat::RGBA8
	};
	m_FinalFramebuffer->Create();

	const uint32_t cubemapSize = 1024;
	const uint32_t irradianceMapSize = 32;

	SharedPtr<Engine::ComputeShader> EquirectangularToCubemapShader = MakeShared<Engine::ComputeShader>("Assets/Shaders/EquirectangularToCubemap.compute.glsl");

	SharedPtr<Engine::TextureCube> environmentTextureCube = MakeShared<Engine::TextureCube>(cubemapSize, cubemapSize);
	SharedPtr<Engine::Texture> HDRTexture = MakeShared<Engine::Texture>("Assets/Environments/Kloppenheim.hdr");

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
	SharedPtr<Engine::TextureCube> filteredEnvironmentTextureCube = MakeShared<Engine::TextureCube>(1024, 1024);

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

	m_RadianceMap = filteredEnvironmentTextureCube;
	m_IrradianceMap = irradianceMap;

	m_SkyboxShader = MakeShared<Engine::Shader>("Assets/Shaders/Skybox.glsl");

	m_BRDFLUTImage = MakeShared<Engine::Texture>("Assets/Textures/BRDF.tga");

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	RendererID skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	m_RoundedCube = MakeShared<Engine::Mesh>("Assets/Meshes/RoundedCube.fbx");
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
		m_MainFramebuffer->Resize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_FinalFramebuffer->Resize(static_cast<u32>(m_ViewportSize.x), static_cast<u32>(m_ViewportSize.y));
		m_ViewportSizeChanged = false;
	}

	MainRenderPass();

	CompositionRenderPass();
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
	BeginDockspace();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N"));
			if (ImGui::MenuItem("Open...", "Ctrl+O"));
			if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"));
			if (ImGui::MenuItem("Exit"));

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Option 1"));
			if (ImGui::MenuItem("Option 2"));
			if (ImGui::MenuItem("Option 3"));
			if (ImGui::MenuItem("Option 4"));

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
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

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Begin("Debug");
	ImGui::Text("Mesh selected: %d", m_IsMeshSelected);
	ImGui::Text("Framerate: %.2f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("Frametime: %.2f ms", 1.0f / ImGui::GetIO().Framerate * 1000.0f);

	ImGui::Checkbox("Render Lines", &m_RenderLines);

	static float lineThickness = 1.0f;
	if (ImGui::SliderFloat("Line Thickness", &lineThickness, 0.1f, 10.0f))
		Engine::Renderer::SetLineThickness(lineThickness);

	static bool enableMSAA = false;
	ImGui::Checkbox("MSAA", &enableMSAA);

	static int samples = 4;
	if (ImGui::SliderInt("Samples", &samples, 2, 32))
	{
		m_MainFramebuffer->samples = samples;
		m_MainFramebuffer->Create();
	}

	ImGui::Separator();

	ImGui::Checkbox("Tonemapping", &m_EnableTonemapping);
	ImGui::SliderFloat("Exposure", &m_Exposure, 0.1f, 10.0f);
	ImGui::SliderFloat("m_TextureLod", &m_TextureLod, 0.0f, 1.0f);

	ImGui::End();

	EndDockspace();
}

void Editor::MainRenderPass()
{
	m_MainFramebuffer->Bind();

	if (m_RenderLines)
		Engine::Renderer::RenderLines();

	if (m_IsMeshSelected)
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Engine::Renderer::SetClearColor(glm::vec4{ 0.7f, 0.7f, 0.7f, 1.0f });
	Engine::Renderer::Clear();

	if (m_IsMeshSelected)
		glStencilMask(0);

	// Render Scene here
	auto& shader = m_TestMesh->GetShader();
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

	m_BRDFLUTImage->Bind(5);
	m_RadianceMap->Bind(6);
	m_IrradianceMap->Bind(7);

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			shader->SetUniformFloat("u_Metalness", i / 10.0f);
			shader->SetUniformFloat("u_Roughness", j / 10.0f);
			Engine::Renderer::SubmitMesh(m_TestMesh, glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.2f - 6.0f, j * 1.2f - 6.0f, 0.0f)));
		}
	}

	shader->SetUniformFloat3("u_AlbedoColor", glm::vec3(0.972f, 0.960f, 0.915f));
	shader->SetUniformFloat("u_Metalness", 1.0f);
	shader->SetUniformFloat("u_Roughness", 0.0f);
	Engine::Renderer::SubmitMeshWithShader(m_RoundedCube, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)) * glm::rotate(glm::mat4(1.0f), (float) glfwGetTime(), glm::vec3(0.0f,1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)), shader);

	if (m_RenderLines)
		Engine::Renderer::RenderLines(false);

	//m_GridShader->Bind();
	//m_GridShader->SetUniformMatrix4("u_ProjectionViewMatrix", m_Camera.GetProjectionViewMatrix());
	//glm::mat4 gridTransform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
	//	glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));
	//m_GridShader->SetUniformMatrix4("u_Transform", gridTransform);
	//m_GridShader->SetUniformFloat3("u_GridColor", glm::vec3(0.4f));
	//m_GridShader->SetUniformFloat("u_Segments", 40.0f);
	//Engine::Renderer::SubmitQuad(m_GridShader);

	glDepthFunc(GL_LEQUAL);

	m_SkyboxShader->Bind();

	glm::mat4 view = glm::mat4(glm::mat3(m_Camera.GetViewMatrix())); // remove translation from the view matrix
	m_SkyboxShader->SetUniformMatrix4("u_Projection", m_Camera.GetProjectionMatrix());
	m_SkyboxShader->SetUniformMatrix4("u_View", view);
	m_SkyboxShader->SetUniformInt("u_ImageCube", 0);
	m_SkyboxShader->SetUniformFloat("u_TextureLod", m_TextureLod);

	m_RadianceMap->Bind(0);

	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);

	// Render selected mesh
	//if (m_IsMeshSelected)
	//{
	//	glStencilFunc(GL_ALWAYS, 1, 0xff);
	//	glStencilMask(0xff);
	//}
	//
	//shader->Bind();
	//shader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	//shader->SetUniformFloat3("u_AlbedoColor", glm::vec3(1.0f, 0.1f, 0.2f));
	//shader->SetUniformFloat("u_Metalness", 0.7f);
	//shader->SetUniformFloat("u_Roughness", 0.2f);
	//
	//shader->SetUniformInt("u_EnableAlbedoTexture", 0);
	//shader->SetUniformInt("u_EnableNormalMapTexture", 0);
	//shader->SetUniformInt("u_EnableMetalnessTexture", 0);
	//shader->SetUniformInt("u_EnableRoughnessTexture", 0);
	//
	//shader->SetUniformInt("u_DirectionalLights[0].Active", 1);
	//shader->SetUniformFloat3("u_DirectionalLights[0].Direction", glm::vec3(glm::radians(30.0f), glm::radians(20.0f), 0.0f));
	//shader->SetUniformFloat3("u_DirectionalLights[0].Radiance", glm::vec3(1.0f));
	//shader->SetUniformFloat("u_DirectionalLights[0].Multiplier", 1.0f);
	//
	//shader->SetUniformInt("u_DirectionalLights[1].Active", 0);
	//shader->SetUniformInt("u_DirectionalLights[2].Active", 0);
	//shader->SetUniformInt("u_DirectionalLights[3].Active", 0);
	//
	//Engine::Renderer::SubmitMesh(m_TestMesh, glm::mat4(1.0f));
	//
	//if (m_IsMeshSelected)
	//{
	//	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	//	glStencilMask(0x00);
	//	glDisable(GL_DEPTH_TEST);
	//
	//	m_OutlineShader->Bind();
	//	m_OutlineShader->SetUniformMatrix4("u_ProjectionView", m_Camera.GetProjectionViewMatrix());
	//	Engine::Renderer::SubmitMeshWithShader(m_TestMesh, glm::scale(glm::mat4(1.0), glm::vec3(1.05f)), m_OutlineShader);
	//
	//	glStencilMask(0xff);
	//	glStencilFunc(GL_ALWAYS, 0, 0xff);
	//	glEnable(GL_DEPTH_TEST);
	//}

	m_MainFramebuffer->UnBind();
}

void Editor::CompositionRenderPass()
{
	m_FinalFramebuffer->Bind();
	m_CompositionShader->Bind();
	
	m_CompositionShader->SetUniformInt("u_Texture", 0);
	m_CompositionShader->SetUniformInt("u_TextureSamples", m_MainFramebuffer->samples);
	m_CompositionShader->SetUniformInt("u_EnableTonemapping", m_EnableTonemapping);
	m_CompositionShader->SetUniformFloat("u_Exposure", m_Exposure);
	glBindTextureUnit(0, m_MainFramebuffer->GetColorAttachmentRendererID());
	
	Engine::Renderer::Clear();
	Engine::Renderer::SubmitQuad(m_CompositionShader);
	
	m_FinalFramebuffer->UnBind();
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
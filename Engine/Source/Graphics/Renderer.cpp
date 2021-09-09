#include "Precompiled.h"
#include "Renderer.h"

#include "GraphicsPipeline.h"
#include "Texture.h"
#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <imgui.h>

#include <unordered_map>


namespace Engine
{
	struct RendererData
	{
		GraphicsPipeline quadPipeline;
		GraphicsPipeline skyboxPipeline;

		std::unordered_map <std::string, SharedPtr<Shader>> shaders;
	};
	static RendererData s_RendererData;

	void Renderer::Initialize()
	{
		ME_INFO("Initializing Renderer");

		float vertices[] = {
			-1.0f, -1.0f, 0.1f,		0.0f, 0.0f,
			 1.0f, -1.0f, 0.1f,		1.0f, 0.0f,
			 1.0f,  1.0f, 0.1f,		1.0f, 1.0f,
			-1.0f,  1.0f, 0.1f,		0.0f, 1.0f
		};
		u32 indices[] = {
			0, 1, 2, 2, 3, 0
		};

		s_RendererData.quadPipeline.layout = {
			{ "a_Position", Engine::VertexFormat::Float3, false },
			{ "a_TexCoords", Engine::VertexFormat::Float2, false }
		};

		s_RendererData.quadPipeline.vertexBuffer = MakeShared<Engine::VertexBuffer>(vertices, sizeof(vertices), Engine::BufferUsage::Static);
		s_RendererData.quadPipeline.indexBuffer = MakeShared<Engine::IndexBuffer>(indices, sizeof(indices), Engine::IndexFormat::Uint32, Engine::BufferUsage::Static);

		s_RendererData.quadPipeline.Create();

		float skyboxVertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			// front face
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,
			// left face
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			// right face
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 // bottom face
			 -1.0f, -1.0f, -1.0f,
			  1.0f, -1.0f, -1.0f,
			  1.0f, -1.0f,  1.0f,
			  1.0f, -1.0f,  1.0f,
			 -1.0f, -1.0f,  1.0f,
			 -1.0f, -1.0f, -1.0f,
			 // top face
			 -1.0f,  1.0f, -1.0f,
			  1.0f,  1.0f , 1.0f,
			  1.0f,  1.0f, -1.0f,
			  1.0f,  1.0f,  1.0f,
			 -1.0f,  1.0f, -1.0f,
			 -1.0f,  1.0f,  1.0f
		};

		s_RendererData.skyboxPipeline.layout = { { "a_Position", VertexFormat::Float3, false } };
		s_RendererData.skyboxPipeline.vertexBuffer = MakeShared<Engine::VertexBuffer>(skyboxVertices, sizeof(skyboxVertices), Engine::BufferUsage::Static);

		s_RendererData.skyboxPipeline.Create();

		s_RendererData.shaders["PBR"] = MakeShared<Shader>("Assets/Shaders/PBR.glsl");
		s_RendererData.shaders["Skybox"] = MakeShared<Shader>("Assets/Shaders/Skybox.glsl");
		s_RendererData.shaders["Grid"] = MakeShared<Shader>("Assets/Shaders/Grid.glsl");
		s_RendererData.shaders["Outline"] = MakeShared<Shader>("Assets/Shaders/Outline.glsl");
		s_RendererData.shaders["Composition"] = MakeShared<Shader>("Assets/Shaders/Composition.glsl");
	}
	void Renderer::Shutdown()
	{
		ME_INFO("Shutting down Renderer");
	}

	SharedPtr<Shader> Renderer::GetShader(const std::string& name)
	{
		return s_RendererData.shaders[name];
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::SetClearColor(const glm::vec4 &clearColor)
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void Renderer::RenderLines(bool enable)
	{
		if(enable)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	void Renderer::SetLineThickness(float thickness)
	{
		glLineWidth(thickness);
	}

	void Renderer::SubmitQuad(const SharedPtr<Shader> &shader)
	{
		s_RendererData.quadPipeline.Bind();
		SubmitPipeline(s_RendererData.quadPipeline);
	}

	void Renderer::SubmitLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec4 color, float thickness)
	{
		ME_ASSERT(false);	// Not implemented
	}

	void Renderer::SubmitMesh(const SharedPtr<Mesh> &mesh, const glm::mat4 &transform)
	{
		auto &pipeline = mesh->m_Pipeline;
		auto &shader = mesh->m_Shader;

		shader->Bind();
		pipeline.Bind();

		for (auto &subMesh : mesh->m_SubMeshes)
		{
			auto &material = mesh->m_Materials[subMesh.materialIndex];
			auto &params = material.GetParameters();
			auto &textures = material.GetTextures();

			shader->SetUniformMatrix4("u_Transform", transform * subMesh.transform);

			shader->SetUniformInt("u_EnableAlbedoTexture", textures.useAlbedo);
			shader->SetUniformInt("u_EnableNormalMapTexture", textures.useNormal);
			shader->SetUniformInt("u_EnableMetalnessTexture", textures.useMetalness);
			shader->SetUniformInt("u_EnableRoughnessTexture", textures.useRoughness);

			shader->SetUniformInt("u_AlbedoTexture", 0);
			shader->SetUniformInt("u_NormalMapTexture", 1);
			shader->SetUniformInt("u_MetalnessTexture", 2);
			shader->SetUniformInt("u_RoughnessTexture", 3);

			if (textures.albedo->IsLoaded())    textures.albedo->Bind(0);
			if (textures.normal->IsLoaded())    textures.normal->Bind(1);
			if (textures.metalness->IsLoaded())	textures.metalness->Bind(2);
			if (textures.roughness->IsLoaded())	textures.roughness->Bind(3);

			shader->SetUniformFloat3("u_AlbedoColor", params.albedo);
			shader->SetUniformFloat("u_Metalness", params.metalness);
			shader->SetUniformFloat("u_Roughness", params.roughness);

			glDrawElementsBaseVertex(GL_TRIANGLES, subMesh.indexCount, GL_UNSIGNED_INT,
				(const void *) (sizeof(u32) * subMesh.indexOffset), subMesh.vertexOffset);
		}
	}

	void Renderer::SubmitMeshWithShader(const SharedPtr<Mesh>& mesh, const glm::mat4& transform, const SharedPtr<Shader>& shader)
	{
		auto& pipeline = mesh->m_Pipeline;

		shader->Bind();
		pipeline.Bind();

		for (auto& subMesh : mesh->m_SubMeshes)
		{
			shader->SetUniformMatrix4("u_Transform", transform * subMesh.transform);

			glDrawElementsBaseVertex(GL_TRIANGLES, subMesh.indexCount, GL_UNSIGNED_INT,
				(const void*)(sizeof(u32) * subMesh.indexOffset), subMesh.vertexOffset);
		}
	}

	void Renderer::SubmitSkybox(const SharedPtr<TextureCube> &skybox, const SharedPtr<Shader> &shader)
	{
		shader->Bind();
		s_RendererData.skyboxPipeline.Bind();

		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
	}

	void Renderer::SubmitPipeline(const GraphicsPipeline &pipeline)
	{
		pipeline.Bind();
		glDrawElements(GL_TRIANGLES, pipeline.indexBuffer->GetCount(),
			pipeline.indexBuffer->GetType(), nullptr);
	}
}
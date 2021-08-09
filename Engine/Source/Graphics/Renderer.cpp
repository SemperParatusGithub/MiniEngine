#include "Precompiled.h"
#include "Renderer.h"

#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include <imgui.h>


namespace Engine
{
	struct RendererData
	{
		GraphicsPipeline quadPipeline;
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

		auto shader = MakeShared<Shader>("Test.glsl");

		s_RendererData.quadPipeline.vertexBuffer = MakeShared<Engine::VertexBuffer>(vertices, sizeof(vertices), Engine::BufferUsage::Static);
		s_RendererData.quadPipeline.indexBuffer = MakeShared<Engine::IndexBuffer>(indices, sizeof(indices), Engine::IndexFormat::Uint32, Engine::BufferUsage::Static);

		s_RendererData.quadPipeline.Create();
	}
	void Renderer::Shutdown()
	{
		ME_INFO("Shutting down Renderer");
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

			//shader->SetUniformInt("u_UseAlbedoMap", textures.useAlbedo);
			//if (textures.albedo->IsLoaded())
			//{
			//	shader->SetUniformInt("u_AlbedoMap", 0);
			//	textures.albedo->Bind(0);
			//}

			shader->SetUniformMatrix4("u_Transform", transform * subMesh.transform);

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

	void Renderer::SubmitPipeline(const GraphicsPipeline &pipeline)
	{
		pipeline.Bind();
		glDrawElements(GL_TRIANGLES, pipeline.indexBuffer->GetCount(),
			pipeline.indexBuffer->GetType(), nullptr);
	}
}
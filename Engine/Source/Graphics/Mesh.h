#pragma once

#include "Core/EngineBase.h"

#include "Material.h"
#include "GraphicsPipeline.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>


namespace Engine
{
	using Index = u32;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texCoords;
	};

	struct Triangle
	{
		Vertex v1, v2, v3;
	};

	struct SubMesh
	{
		u32 vertexOffset, indexOffset;
		u32 vertexCount, indexCount;
		u32 materialIndex;
		glm::mat4 transform;
	};

	class Mesh
	{
	public:
		Mesh();
		Mesh(ConstRef<std::string> filepath);
		~Mesh();

		void Load(const std::string &filepath);

		bool IsLoaded() const;
		std::vector<Material> &GetMaterials();
		std::vector<SubMesh> &GetSubMeshes();

		const std::string& GetFilepath() const { return m_Filepath; }

		SharedPtr<Shader> GetShader() { return m_Shader; }

		auto begin() const noexcept { return m_SubMeshes.begin(); }
		auto end() const noexcept { return m_SubMeshes.end(); }

		const std::vector<Triangle>& GetTriangleRepresentation() const
		{
			return m_TriangleRepresentation;
		}

	private:
		void ProcessNode(aiNode *node, ConstRef<glm::mat4> parenTransform);
		SubMesh ProcessMesh(aiMesh *mesh, ConstRef<glm::mat4> meshTransform);

		void PreparePipeline();

	private:
		std::string m_Filepath;
		bool m_IsLoaded;
		std::vector<SubMesh> m_SubMeshes;

		SharedPtr<Shader> m_Shader;
		std::vector<Material> m_Materials;

		const aiScene *m_Scene;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		std::vector<Triangle> m_TriangleRepresentation;

		// Pipeline
		GraphicsPipeline m_Pipeline;

		friend class Renderer;
	};
}
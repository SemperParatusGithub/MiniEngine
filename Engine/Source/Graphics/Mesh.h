#pragma once
#include "Core/EngineBase.h"

#include "Material.h"

#include <vector>

#include <glm/glm.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>


namespace Engine
{
	class Renderer;
	class GraphicsPipeline;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texCoords;
	};

	class Submesh
	{
	public:
		u32 vertexOffset, indexOffset;
		u32 vertexCount, indexCount;
		u32 materialIndex;
		glm::mat4 transform;
	};

	using Index = u32;


	class Mesh
	{
	public:
		Mesh(const std::string &filepath);
		Mesh();

		~Mesh();

		void Load(const std::string &filepath);
		bool IsLoaded() const;

		SharedPtr<Shader> GetShader();
		const std::string &GetFilepath() const;

		const std::vector<Material> &GetMaterials() const;
		std::vector<Material> &GetMaterials();

		void SetFlipUVs(bool flip);

	private:
		void ProcessNode(aiNode *node, const glm::mat4 &parentTransform = glm::mat4(1.0f));
		void ProcessMesh(aiMesh *mesh, const glm::mat4 &transform);
		void TraverseNodes(aiNode *node, const glm::mat4 &parentTransform = glm::mat4(1.0f), uint32_t level = 0);

	private:
		std::string m_Filepath;
		bool m_IsLoaded;
		bool m_FlipUVs;

		std::vector<Submesh> m_SubMeshes;
		GraphicsPipeline m_Pipeline;
		SharedPtr<Shader> m_Shader;

		std::vector<Material> m_Materials;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		const aiScene *m_Scene;
		UniquePtr<Assimp::Importer> m_Importer;

		friend class Renderer;
	};
}
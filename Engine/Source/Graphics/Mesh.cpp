#include "Precompiled.h"
#include "Engine.h"

#include "Renderer.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"


#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>


namespace Engine
{
	glm::vec3 AssimpVec3ToVec3(ConstRef<aiVector3D> vector)
	{
		return glm::vec3 {
			vector.x,
			vector.y,
			vector.z
		};
	}
	glm::mat4 AssimpMat4ToMat4(const aiMatrix4x4 &matrix)
	{
		return glm::mat4 {
			{ matrix.a1, matrix.a2, matrix.a3, matrix.a4 },
			{ matrix.b1, matrix.b2, matrix.b3, matrix.b4 },
			{ matrix.c1, matrix.c2, matrix.c3, matrix.c4 },
			{ matrix.d1, matrix.d2, matrix.d3, matrix.d4 }
		};
	}

	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace             |
		aiProcess_Triangulate                  |
		aiProcess_SortByPType                  |
		aiProcess_GenNormals                   |
		aiProcess_GenUVCoords                  |
		aiProcess_OptimizeMeshes               |
		aiProcess_ValidateDataStructure;

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Info | Assimp::Logger::Warn | Assimp::Logger::Err);
			}
		}

		virtual void write(const char *message) override
		{
			std::string msg = std::string(message);
			std::replace(msg.begin(), msg.end(), '\n', ' ');

			if (msg.find("Info") != std::string::npos)
			{
				auto pos = msg.find("Info");
				msg.replace(pos, pos + 4 + 3, "");
				ME_INFO("Assimp: %s", msg.c_str());
			}
			if (msg.find("Warn") != std::string::npos)
			{
				auto pos = msg.find("Warn");
				msg.replace(pos, pos + 4 + 3, "");
				ME_WARN("Assimp: %s", msg.c_str());
			}
			if (msg.find("Error") != std::string::npos)
			{
				auto pos = msg.find("Error");
				msg.replace(pos, pos + 5 + 3, "");
				ME_ERROR("Assimp: %s", msg.c_str());
			}
		}
	};

	Mesh::Mesh(const std::string &filepath)
	{
		Load(filepath);
	}
	Mesh::Mesh() : 
		m_Filepath(""),
		m_IsLoaded(false),
		m_FlipUVs(false),
		m_Scene(nullptr)
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Load(const std::string &filepath)
	{
		LogStream::Initialize();

		m_Filepath = filepath;
		ME_INFO("Loading mesh: %s", filepath.c_str());

		m_FlipUVs = false;

		m_Importer = MakeUnique<Assimp::Importer>();

		const aiScene *scene = m_Importer->ReadFile(filepath, s_MeshImportFlags);
		m_Scene = scene;

		if (!scene || !scene->HasMeshes())
		{
			ME_ERROR("Failed to load mesh file: %s", filepath.c_str());
			m_IsLoaded = false;
		}
		else {
			ProcessNode(m_Scene->mRootNode);

			m_Shader = MakeShared<Shader>("Mesh.glsl");

			m_Pipeline.layout = {
				{ "a_Position", VertexFormat::Float3, false },
				{ "a_Normal",   VertexFormat::Float3, false },
				{ "a_Tangent",  VertexFormat::Float3, false },
				{ "a_Binormal", VertexFormat::Float3, false },
				{ "a_TexCoord", VertexFormat::Float2, false }
			};

			m_Pipeline.vertexBuffer = MakeShared<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), BufferUsage::Static);
			m_Pipeline.indexBuffer = MakeShared<IndexBuffer>(m_Indices.data(), m_Indices.size() * sizeof(Index), IndexFormat::Uint32, BufferUsage::Static);

			m_Pipeline.Create();

			m_IsLoaded = true;
		}
	}

	bool Mesh::IsLoaded() const
	{
		return m_IsLoaded;
	}

	SharedPtr<Shader> Mesh::GetShader()
	{
		return m_Shader;
	}
	const std::string &Mesh::GetFilepath() const
	{
		return m_Filepath;
	}

	const std::vector<Material> &Mesh::GetMaterials() const
	{
		return m_Materials;
	}
	std::vector<Material> &Mesh::GetMaterials()
	{
		return m_Materials;
	}

	void Mesh::SetFlipUVs(bool flip)
	{
		m_FlipUVs = flip;
	}

	void Mesh::ProcessNode(aiNode *node, const glm::mat4 &parentTransform)
	{
		glm::mat4 transform = parentTransform * AssimpMat4ToMat4(node->mTransformation);

		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = m_Scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, transform);
		}
		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], transform);
		}
	}

	void Mesh::ProcessMesh(aiMesh *mesh, const glm::mat4 &transform)
	{
		auto &subMesh = m_SubMeshes.emplace_back();
		subMesh.transform = transform;
		subMesh.vertexOffset = m_Vertices.size();
		subMesh.indexOffset = m_Indices.size();
		subMesh.materialIndex = m_Materials.size();

		// Vertices
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex currentVertex;

			// Position
			currentVertex.position = AssimpVec3ToVec3(mesh->mVertices[i]);

			// Normals
			if (mesh->HasNormals())
				currentVertex.normal = AssimpVec3ToVec3(mesh->mNormals[i]);

			// Tangents & Bitangents
			if (mesh->HasTangentsAndBitangents())
			{
				currentVertex.tangent = AssimpVec3ToVec3(mesh->mTangents[i]);
				currentVertex.bitangent = AssimpVec3ToVec3(mesh->mBitangents[i]);
			}

			// Texture Coords
			if (mesh->HasTextureCoords(0))
			{
				currentVertex.texCoords = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}

			m_Vertices.push_back(currentVertex);
		}

		// Indices
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; j++)
				m_Indices.push_back(face.mIndices[j]);
		}

		// handle offsets
		subMesh.vertexCount = m_Vertices.size() - subMesh.vertexOffset;
		subMesh.indexCount = m_Indices.size() - subMesh.indexOffset;

		// Materials
		{
			aiMaterial *aiMaterial = m_Scene->mMaterials[mesh->mMaterialIndex];

			u32 materialIndex = mesh->mMaterialIndex;
			std::string materialName = aiMaterial->GetName().C_Str();

			auto &material = m_Materials.emplace_back(materialName);

			aiColor3D aiColor;
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);

			float shininess, metalness, roughness;
			if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS)
				shininess = 80.0f; // Default value

			if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS)
				metalness = 0.0f;

			roughness = 1.0f - glm::sqrt(shininess / 100.0f);

			material.GetParameters() = {
				glm::vec3(aiColor.r, aiColor.g, aiColor.b),
				metalness,
				roughness
			};

			ME_TRACE("Mesh Debug Info: %s: AlbedoColor: %.2f, %.2f, %.2f", aiMaterial->GetName().C_Str(), aiColor.r, aiColor.g, aiColor.b);
			ME_TRACE("Mesh Debug Info: %s: Metalness: %.2f", aiMaterial->GetName().C_Str(), metalness);
			ME_TRACE("Mesh Debug Info: %s: Roughness: %.2f", aiMaterial->GetName().C_Str(), roughness);

			auto &textures = material.GetTextures();

			aiString aiTexturePath;
			if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS)
			{
				std::filesystem::path path = m_Filepath;
				auto parentPath = path.parent_path();
				parentPath /= std::string(aiTexturePath.data);
				std::string texturePath = parentPath.string();
				ME_TRACE("Albedo Texture filepath = %s", texturePath.c_str());

				textures.albedo = MakeShared<Texture>(texturePath, true);
				if (textures.albedo->IsLoaded())
					textures.useAlbedo = true;
			}
		}
	}
}
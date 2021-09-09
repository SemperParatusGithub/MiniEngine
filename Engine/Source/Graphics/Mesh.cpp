#include "Precompiled.h"
#include "Mesh.h"

#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>

#include "Renderer.h"


namespace Engine
{
	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes |
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

	glm::vec3 AssimpVec3ToVec3(ConstRef<aiVector3D> vector)
	{
		return glm::vec3 {
			vector.x,
			vector.y,
			vector.z
		};
	}
	glm::vec2 AssimpVec2ToVec2(ConstRef<aiVector2D> vector)
	{
		return	glm::vec2 {
			vector.x,
			vector.y
		};
	}

	glm::mat4 AssimpMat4ToMat4(ConstRef<aiMatrix4x4> matrix)
	{
		glm::mat4 result;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
		result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
		result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
		result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
		return result;
	}

	Mesh::Mesh() :
		m_Filepath(""),
		m_IsLoaded(false),
		m_Scene(nullptr)
	{
	}
	Mesh::Mesh(ConstRef<std::string> filepath) :
		m_Filepath(filepath),
		m_IsLoaded(false),
		m_Scene(nullptr)
	{
		Load(filepath);
	}
	Mesh::~Mesh()
	{
	}
	void Mesh::Load(ConstRef<std::string> filepath)
	{
		ME_INFO("Loading Mesh: %s", filepath.c_str());

		LogStream::Initialize();

		m_Filepath = filepath;
		m_SubMeshes.clear();

		UniquePtr<Assimp::Importer> m_Importer;
		m_Importer = MakeUnique<Assimp::Importer>();
		m_Scene = m_Importer->ReadFile(filepath, s_MeshImportFlags);

		if (!m_Scene || !m_Scene->HasMeshes())
		{
			ME_ERROR("Failed to load mesh: %s", m_Filepath.c_str());
		}
		else if (m_Scene->mAnimations)
		{
			ME_ERROR("Animations currently not supported: %s", m_Filepath.c_str());
		}
		else {
			// Process mesh recursively
			ProcessNode(m_Scene->mRootNode, glm::mat4(1.0f));

			ME_TRACE("Total sub meshes: %d", m_SubMeshes.size());
			ME_TRACE("Total mesh vertices: %d", m_Vertices.size());
			ME_TRACE("Total mesh indices: %d", m_Indices.size());

			ME_INFO("Preparing Pipeline");
			PreparePipeline();
			ME_INFO("Pipeline was succesfully prepared");

			m_IsLoaded = true;
		}
	}

	bool Mesh::IsLoaded() const
	{
		return m_IsLoaded;
	}

	std::vector<Material> &Mesh::GetMaterials()
	{
		return m_Materials;
	}

	std::vector<SubMesh> &Mesh::GetSubMeshes()
	{
		return m_SubMeshes;
	}

	void Mesh::ProcessNode(aiNode *node, ConstRef<glm::mat4> parentTransform)
	{
		glm::mat4 transform = parentTransform * AssimpMat4ToMat4(node->mTransformation);

		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = m_Scene->mMeshes[node->mMeshes[i]];
			m_SubMeshes.push_back(ProcessMesh(mesh, transform));
		}
		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], transform);
		}
	}
	SubMesh Mesh::ProcessMesh(aiMesh *mesh, ConstRef<glm::mat4> meshTransform)
	{
		SubMesh subMesh;
		subMesh.transform = meshTransform;
		subMesh.vertexOffset = m_Vertices.size();
		subMesh.indexOffset = m_Indices.size();

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
				glm::vec2 textureCoords = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};

				currentVertex.texCoords = textureCoords;
			}

			m_Vertices.push_back(currentVertex);
		}

		// Indices
		typedef struct { u32 v1, v2, v3; } TriangleInfo;
		
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			ME_ASSERT(face.mNumIndices == 3);

			TriangleInfo currentTriangle;
			currentTriangle = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };

			// Triangles
			Triangle triangle = {
				m_Vertices[currentTriangle.v1 + subMesh.vertexOffset],
				m_Vertices[currentTriangle.v2 + subMesh.vertexOffset],
				m_Vertices[currentTriangle.v3 + subMesh.vertexOffset]
			};
			m_TriangleRepresentation.emplace_back(triangle);

			for (uint32_t j = 0; j < face.mNumIndices; j++)
				m_Indices.push_back(face.mIndices[j]);
		}

		// handle offsets
		subMesh.vertexCount = m_Vertices.size() - subMesh.vertexOffset;
		subMesh.indexCount = m_Indices.size() - subMesh.indexOffset;

		// Materials
		{
			aiMaterial* aiMaterial = m_Scene->mMaterials[mesh->mMaterialIndex];

			u32 materialIndex = m_Materials.size();
			std::string materialName = aiMaterial->GetName().C_Str();
			bool materialFound = false;

			for (int i = 0; i < m_Materials.size(); i++)
			{
				if (m_Materials[i].GetName() == materialName)
				{
					materialIndex = i;
					materialFound = true;
				}
			}

			if (!materialFound)
			{
				Material subMaterial(materialName);

				aiColor3D aiColor;
				aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);

				float shininess, metalness, roughness;
				if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS)
					shininess = 80.0f; // Default value

				if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS)
					metalness = 0.0f;

				roughness = 1.0f - glm::sqrt(shininess / 100.0f);

				subMaterial.GetParameters() = {
					glm::vec3(aiColor.r, aiColor.g, aiColor.b),	// Albedo
					metalness,									// Metalness	
					roughness									// Roughness
				};

				ME_TRACE("Mesh Debug Info: %s: AlbedoColor: %.2f, %.2f, %.2f", aiMaterial->GetName().C_Str(), aiColor.r, aiColor.g, aiColor.b);
				ME_TRACE("Mesh Debug Info: %s: Metalness: %.2f", aiMaterial->GetName().C_Str(), metalness);
				ME_TRACE("Mesh Debug Info: %s: Roughness: %.2f", aiMaterial->GetName().C_Str(), roughness);

				// TODO: Normal, metalness and roughness maps
				PBRMaterialTextures textures = {
					MakeShared<Texture>(), false,
					MakeShared<Texture>(), false,
					MakeShared<Texture>(), false,
					MakeShared<Texture>(), false
				};

				aiString aiTexturePath;
				if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS)
				{
					std::filesystem::path path = m_Filepath;
					auto parentPath = path.parent_path();
					parentPath /= std::string(aiTexturePath.data);
					std::string texturePath = parentPath.string();
					ME_INFO("Albedo Texture filepath = %s", texturePath.c_str());
					auto texture = MakeShared<Texture>(texturePath, true);
					textures.albedo = texture;
					if (texture->IsLoaded())
						textures.useAlbedo = true;
				}

				subMaterial.GetTextures() = textures;


				m_Materials.push_back(subMaterial);
			}

			subMesh.materialIndex = materialIndex;
		}

		return subMesh;
	}

	void Mesh::PreparePipeline()
	{
		m_Pipeline.layout = {
			{ "a_Position",  VertexFormat::Float3, false },
			{ "a_Normal",	 VertexFormat::Float3, false },
			{ "a_Tangent",	 VertexFormat::Float3, false },
			{ "a_Bitangent", VertexFormat::Float3, false },
			{ "a_TexCoords", VertexFormat::Float2, false }
		};

		m_Pipeline.vertexBuffer = MakeShared<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), BufferUsage::Static);
		m_Pipeline.indexBuffer = MakeShared<IndexBuffer>(m_Indices.data(), m_Indices.size() * sizeof(u32), IndexFormat::Uint32, BufferUsage::Static);

		m_Pipeline.Create();

		m_Shader = Renderer::GetShader("PBR");
	}
}
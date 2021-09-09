#pragma once
#include "Core/EngineBase.h"
#include "Texture.h"

#include <glm/glm.hpp>


namespace Engine
{
	struct PBRMaterialTextures
	{
		SharedPtr<Texture> albedo;
		bool useAlbedo = false;

		SharedPtr<Texture> normal;
		bool useNormal = false;

		SharedPtr<Texture> metalness;
		bool useMetalness = false;

		SharedPtr<Texture> roughness;
		bool useRoughness = false;
	};
	struct PBRMaterialParameters
	{
		glm::vec3 albedo = { 0.8f, 0.2f, 0.15f };
		float metalness = 0.5f;
		float roughness = 0.5f;
	};

	class Material
	{
	public:
		Material();
		Material(const std::string &name);
		~Material();

		// TODO: Material Flags

		void SetName(const std::string &name);
		const std::string &GetName() const;

		PBRMaterialParameters &GetParameters();
		const PBRMaterialParameters &GetParameters() const;

		PBRMaterialTextures &GetTextures();
		const PBRMaterialTextures &GetTextures() const;

	private:
		std::string m_Name;
		PBRMaterialParameters m_Parameters;
		PBRMaterialTextures m_Textures;
	};
}
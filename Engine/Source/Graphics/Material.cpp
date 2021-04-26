#include "Precompiled.h"
#include "Material.h"


namespace Engine
{
	Material::Material() : 
		m_Name("Unknown Material")
	{
	}
	Material::Material(const std::string &name) : 
		m_Name(name)
	{
	}
	Material::~Material()
	{
	}

	void Material::SetName(const std::string &name)
	{
		m_Name = name;
	}
	const std::string &Material::GetName() const
	{
		return m_Name;
	}

	PBRMaterialParameters &Material::GetParameters()
	{
		return m_Parameters;
	}
	const PBRMaterialParameters &Material::GetParameters() const
	{
		return m_Parameters;
	}

	PBRMaterialTextures &Material::GetTextures()
	{
		return m_Textures;
	}
	const PBRMaterialTextures &Material::GetTextures() const
	{
		return m_Textures;
	}
}
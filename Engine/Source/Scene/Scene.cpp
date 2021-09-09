#include "Precompiled.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

namespace Engine
{
	Entity Scene::CreateEntity()
	{
		ME_INFO("Creating empty Entity");

		auto handle = m_Registry.create();
		auto entity = Entity(handle, this);

		entity.Add<IDComponent>(IDComponent{ "Unknown" });
		entity.Add<TransformComponent>(TransformComponent{});

		return entity;
	}
	Entity Scene::CreateEntity(const std::string name)
	{
		ME_INFO("Creating Entity: %s", name.c_str());

		auto handle = m_Registry.create();
		auto entity = Entity(handle, this);

		entity.Add<IDComponent>(IDComponent{ name });
		entity.Add<TransformComponent>(TransformComponent{});

		return entity;
	}
}
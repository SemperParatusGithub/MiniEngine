#pragma once
#include "Core/EngineBase.h"

#include "Scene.h"

#include <entt/entt.hpp>


namespace Engine
{
	class Scene;

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene) :
			m_Entity(entity), m_Scene(scene)
		{
		}

		entt::entity GetEntity() const { return m_Entity; }
		Scene* GetScene() const { return m_Scene; }

		bool IsValid() const
		{
			if (m_Scene && m_Entity != entt::null)
				return m_Scene->GetRegistry().valid(m_Entity);
			else
				return false;
		}
		operator bool() const {	return IsValid(); }

		inline void Destroy()
		{
			ME_ASSERT(IsValid());	// Invalid Entity

			m_Scene->GetRegistry().destroy(m_Entity);
			m_Scene = nullptr;
		}

		template<typename Component, typename ... Args>
		inline Component& Add(Args && ... args)
		{
			ME_ASSERT(!Has<Component>());	// Entity already has component

			return m_Scene->GetRegistry().emplace<Component>(m_Entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		inline void Remove()
		{
			ME_ASSERT(Has<Component>());	// Component doesn't exist

			m_Scene->GetRegistry().remove<Component>(m_Entity);
		}

		template<typename Component>
		inline Component& Get()
		{
			ME_ASSERT(Has<Component>());	// Component doesn't exist

			return m_Scene->GetRegistry().get<Component>(m_Entity);
		}

		template<typename Component>
		inline ConstRef<Component> Get() const
		{
			ME_ASSERT(Has<Component>()); // Component doesn't exist

			return m_Scene->GetRegistry().get<Component>(m_Entity);
		}

		template<typename ... Components>
		inline bool Has() const
		{
			return m_Scene->GetRegistry().has<Components...>(m_Entity);
		}

	private:
		entt::entity m_Entity { entt::null };
		Scene* m_Scene { nullptr };
	};
}
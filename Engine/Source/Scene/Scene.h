#pragma once
#include "Core/EngineBase.h"
#include "Core/Event.h"

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include <array>


class b2World;

namespace Engine
{
	class TextureCube;
	class Texture;

	struct DirectionalLight
	{
		bool active { false };
		glm::vec3 direction { 0.5f, 0.25f, 0.0f };
		glm::vec3 radiance { 1.0f };
		float intensity { 1.0f };
	};

	struct Environment
	{
		SharedPtr<TextureCube> radianceMap;
		SharedPtr<TextureCube> irradianceMap;
		SharedPtr<Texture> brdflutTexture;
		DirectionalLight directionalLight;
		float textureLod = 0.0f;
		float exposure = 1.0f;
	};


	class Entity;

	class Scene
	{
	public:
		static void Copy(SharedPtr<Scene> source, SharedPtr<Scene> destination);

	public:
		Scene() {}
		~Scene() {}

		Scene(const SharedPtr<Scene> &src);

		Entity CreateEntity();
		Entity CreateEntity(const std::string name);
		Entity DuplicateEntity(Entity entity);

		const entt::registry& GetRegistry() const { return m_Registry; }
		entt::registry &GetRegistry() { return m_Registry; }

		void SetupPhysicsSimulation();
		void OnUpdate(float delta);

	private:
		void CopyRegistry(entt::registry& from, entt::registry& to);

	public:
		Environment environment;

	private:
		entt::registry m_Registry;
		b2World* m_PhysicsWorld { nullptr };
	};
}
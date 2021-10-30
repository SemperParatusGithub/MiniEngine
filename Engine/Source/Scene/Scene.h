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

	enum class SceneState
	{
		Editing = 0,
		Playing,
		Pausing
	};


	class Entity;

	class Scene
	{
	public:
		Scene() {}
		~Scene() {}

		Entity CreateEntity();
		Entity CreateEntity(const std::string name);

		const entt::registry& GetRegistry() const { return m_Registry; }
		entt::registry &GetRegistry() { return m_Registry; }

		void OnUpdate(float delta);
		void OnEvent(Event& e);

		void Play();
		void Pause();
		void Reset();

	public:
		Environment environment;

	private:
		SceneState m_SceneState = SceneState::Editing;

		entt::registry m_Registry;
		b2World* m_PhysicsWorld { nullptr };
	};
}
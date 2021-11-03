#include "Precompiled.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

// Box2D
#include <box2d/box2d.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>


namespace Engine
{
	static b2BodyType RigidBody2DToBox2D(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		ME_ASSERT(false); // Unknown body type
		return b2_staticBody;
	}

	template<typename Component>
	void ReplaceComponent(entt::registry& from, entt::entity entFrom, entt::registry& to, entt::entity entTo)
	{
		if (from.has<Component>(entFrom))
			to.emplace_or_replace<Component>(entTo, from.get<Component>(entFrom));
	}

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

	Entity Scene::DuplicateEntity(Entity entity)
	{
		auto& id = entity.Get<IDComponent>();
		auto ent = CreateEntity(id.name);

		ReplaceComponent<TransformComponent>(m_Registry, entity.GetEntity(), m_Registry, ent.GetEntity());
		ReplaceComponent<MeshComponent>(m_Registry, entity.GetEntity(), m_Registry, ent.GetEntity());
		ReplaceComponent<Rigidbody2DComponent>(m_Registry, entity.GetEntity(), m_Registry, ent.GetEntity());
		ReplaceComponent<BoxCollider2DComponent>(m_Registry, entity.GetEntity(), m_Registry, ent.GetEntity());
		ReplaceComponent<CircleCollider2DComponent>(m_Registry, entity.GetEntity(), m_Registry, ent.GetEntity());

		return ent;
	}

	void Scene::OnUpdate(float delta)
	{
		if (m_SceneState != SceneState::Playing)
			return;

		// Update Physics
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_PhysicsWorld->Step(delta, velocityIterations, positionIterations);

		// Retrieve transform from Box2D
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.Get<TransformComponent>().transform;
			auto& rb2d = entity.Get<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			const auto& position = body->GetPosition();
			transform.SetTranslation({ position.x, position.y, transform.GetTranslation().z });
			transform.SetRotation({ transform.GetRotation().x, transform.GetRotation().y, body->GetAngle() });
		}
	}
	void Scene::OnEvent(Event& e)
	{
	}

	void Scene::Play()
	{
		if (m_SceneState == SceneState::Playing)
			return;

		if (m_SceneState == SceneState::Pausing) {
			m_SceneState = SceneState::Playing;
			return;
		}

		CopyRegistry(m_Registry, m_BackupRegistry);

		ME_INFO("Starting Scene");
		m_SceneState = SceneState::Playing;

		m_PhysicsWorld = new b2World({ 0.0f, -9.81f });

		auto view = m_Registry.view<Rigidbody2DComponent>();

		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.Get<TransformComponent>().transform;
			auto& rb2d = entity.Get<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = RigidBody2DToBox2D(rb2d.Type);
			bodyDef.position.Set(transform.GetTranslation().x, transform.GetTranslation().y);
			bodyDef.angle = transform.GetRotation().z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(false);	// TODO: physics material property
			rb2d.RuntimeBody = body;

			if (entity.Has<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.Get<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.GetScale().x, bc2d.Size.y * transform.GetScale().y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.Has<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.Get<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_radius = cc2d.Radius * transform.GetScale().x;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}
	void Scene::Pause()
	{
		if (m_SceneState != SceneState::Playing)	// Can't pause when we are not playing
			return;
		ME_INFO("Pausing Scene");

		m_SceneState = SceneState::Pausing;
	}
	void Scene::Reset()
	{
		if (m_SceneState == SceneState::Editing)
			return;
		ME_INFO("Resetting Scene");
		m_SceneState = SceneState::Editing;

		CopyRegistry(m_BackupRegistry, m_Registry);
	}

	void Scene::CopyRegistry(entt::registry& from, entt::registry& to)
	{
		to.clear();

		auto all = from.view<IDComponent>();
		for (auto& currentEntity : all)
		{
			auto newEnt = to.create();

			ReplaceComponent<IDComponent>(from, currentEntity, to, newEnt);
			ReplaceComponent<TransformComponent>(from, currentEntity, to, newEnt);
			ReplaceComponent<MeshComponent>(from, currentEntity, to, newEnt);
			ReplaceComponent<Rigidbody2DComponent>(from, currentEntity, to, newEnt);
			ReplaceComponent<BoxCollider2DComponent>(from, currentEntity, to, newEnt);
			ReplaceComponent<CircleCollider2DComponent>(from, currentEntity, to, newEnt);
		}
	}
}
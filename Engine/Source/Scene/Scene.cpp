#include "Precompiled.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

// Box2D
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
		}
	}
	void Scene::Pause()
	{
		ME_INFO("Pausing Scene");
		ME_ASSERT(false);	// Not implemented
	}
	void Scene::Reset()
	{
		ME_INFO("Resetting Scene");
		m_SceneState = SceneState::Editing;
	}
}
#pragma once
#include "Core/EngineBase.h"

#include "Core/UUID.h"

#include "Graphics/Transform.h"
#include "Graphics/Mesh.h"

#include "Graphics/Camera.h"


namespace Engine
{
	struct IDComponent
	{
		IDComponent() : name("Unknown") {}
		IDComponent(const std::string &name) : name(name) {}

		UUID uuid;
		std::string name;
	};

	struct TransformComponent
	{
		Transform transform;
	};

	struct MeshComponent
	{
		SharedPtr<Mesh> mesh;

		MeshComponent() {
			mesh = MakeShared<Mesh>();
		}
		MeshComponent(const std::string &filepath) {
			mesh = MakeShared<Mesh>(filepath);
		}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = false;

		const glm::mat4 &GetProjectionViewMatrix()
		{
			return camera.GetProjectionViewMatrix();
		}
		void SetBounds(float width, float height)
		{
			camera.SetBounds(width, height);
		}
	};


	// Physics Components
	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;

		// Storage for runtime 
		// TODO: Move to Scene entity map
		void* RuntimeBody = nullptr;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		// TODO: Move to Scene entity map
		void* RuntimeFixture = nullptr;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f,0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 1.0f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		// TODO: Move to Scene entity map
		void* RuntimeFixture = nullptr;
	};
}
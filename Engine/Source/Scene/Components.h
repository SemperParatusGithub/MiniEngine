#pragma once
#include "Core/EngineBase.h"

#include "Graphics/Transform.h"
#include "Graphics/Mesh.h"


namespace Engine
{
	struct IDComponent
	{
		IDComponent() : name("Unknown") {}
		IDComponent(const std::string &name) : name(name) {}

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
}
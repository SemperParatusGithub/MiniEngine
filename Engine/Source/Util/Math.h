#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>


namespace Engine
{
	struct Ray
	{
		glm::vec3 origin;
		glm::vec3 direction;
	};
	struct Triangle;

	class Math
	{
	public:
		static bool RayIntersectsTriangle(Ray ray, Triangle triangle);

	};
}
#include "Precompiled.h"
#include "Math.h"

#include "Graphics/Mesh.h"


namespace Engine
{
	// Checks if ray intersects with triangle by using the Möller–Trumbore intersection algorithm
	// Wikipedia: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm 

	bool Math::RayIntersectsTriangle(Ray ray, Triangle triangle, float &distance)
	{
		constexpr float EPSILON = 0.0000001;

		glm::vec3 vertex0 = triangle.v1.position;
		glm::vec3 vertex1 = triangle.v2.position;
		glm::vec3 vertex2 = triangle.v3.position;

		glm::vec3 edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = vertex1 - vertex0;
		edge2 = vertex2 - vertex0;

		h = glm::cross(ray.direction, edge2);
		a = glm::dot(edge1, h);

		// ray is parallel to triangle
		if (a > -EPSILON && a < EPSILON)
			return false;

		f = 1.0 / a;
		s = ray.origin - vertex0;
		u = f * glm::dot(s, h);

		if (u < 0.0 || u > 1.0)
			return false;

		q = glm::cross(s, edge1);
		v = f * glm::dot(ray.direction, q);

		if (v < 0.0 || u + v > 1.0)
			return false;

		// at this stage we can compute t to find out where the intersection point is on the line.
		float t = f * glm::dot(edge2, q);

		// ray intersection
		if (t > EPSILON) {
			distance = glm::length(ray.origin + ray.direction * t);
			return true;
		}
		// This means that there is a line intersection but not a ray intersection.
		else
			return false;
	}
}
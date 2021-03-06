#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/constants.hpp>


// from <glm/gtx/decomposition.hpp>

namespace glm
{
	namespace detail
	{
		/// Make a linear combination of two vectors and return the result.
		// result = (a * ascl) + (b * bscl)
		template<typename T, qualifier Q>
		GLM_FUNC_QUALIFIER vec<3, T, Q> combine(
			vec<3, T, Q> const& a,
			vec<3, T, Q> const& b,
			T ascl, T bscl)
		{
			return (a * ascl) + (b * bscl);
		}

		template<typename T, qualifier Q>
		GLM_FUNC_QUALIFIER vec<3, T, Q> scale(vec<3, T, Q> const& v, T desiredLength)
		{
			return v * desiredLength / length(v);
		}
	} //namespace detail
} //namespace glm


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
		static bool RayIntersectsTriangle(Ray ray, Triangle triangle, float &distance);

		static inline std::tuple<glm::vec3, glm::vec3, glm::vec3> Decompose(const glm::mat4 &transform)
		{
			glm::vec3 translation, rotation, scale;
			glm::mat4 localMatrix(transform);

			// Normalize the matrix.
			if (glm::epsilonEqual(localMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
				ME_ASSERT(false); // Failed to normalized Matrix

			// First, isolate perspective.  This is the messiest.
			if (
				glm::epsilonNotEqual(localMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
				glm::epsilonNotEqual(localMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
				glm::epsilonNotEqual(localMatrix[2][3], static_cast<float>(0), glm::epsilon<float>()))
			{
				// Clear the perspective partition
				localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<float>(0);
				localMatrix[3][3] = static_cast<float>(1);
			}

			// Next take care of translation (easy).
			translation = glm::vec3(localMatrix[3]);
			localMatrix[3] = glm::vec4(0, 0, 0, localMatrix[3].w);

			glm::vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (glm::length_t i = 0; i < 3; ++i)
				for (glm::length_t j = 0; j < 3; ++j)
					Row[i][j] = localMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));
			scale.y = length(Row[1]);
			Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));
			scale.z = length(Row[2]);
			Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0) {
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else {
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}

			return std::tuple{
				translation,
				rotation,
				scale
			};
		}


	};
}
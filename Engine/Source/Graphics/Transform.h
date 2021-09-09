#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>

namespace Engine
{
	// Rotation is always in radians

	class Transform
	{
	public:
		Transform();
		Transform(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

		const glm::vec3& GetTranslation() const noexcept;
		const glm::vec3& GetRotation() const noexcept;
		const glm::vec3& GetScale() const noexcept;		

		void SetTranslation(const glm::vec3& translation);
		void SetRotation(const glm::vec3& rotation);
		void SetScale(const glm::vec3& scale);

		void Translate(const glm::vec3& offset);
		void Rotate(const glm::vec3& offset);
		void Scale(const glm::vec3& offset);

		const glm::mat4& GetTransform() const noexcept;


	private:
		void RecalculateTransformationMatrices();

	private:
		glm::vec3 m_Translation, m_Rotation, m_Scale;
		glm::mat4 m_TransformationMatrix;
	};
}
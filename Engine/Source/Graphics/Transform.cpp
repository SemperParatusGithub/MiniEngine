#include "Precompiled.h"
#include "Transform.h"

#include "Precompiled.h"
#include "Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Engine
{
	Transform::Transform() :
		m_Translation(0.0f),
		m_Rotation(0.0f),
		m_Scale(1.0f)
	{
		RecalculateTransformationMatrices();
	}

	Transform::Transform(const glm::vec3 & translation, const glm::vec3 & rotation, const glm::vec3 & scale) :
		m_Translation(translation),
		m_Rotation(rotation),
		m_Scale(scale)
	{
		RecalculateTransformationMatrices();
	}

	const glm::vec3 &Transform::GetTranslation() const noexcept
	{
		return m_Translation;
	}
	const glm::vec3 &Transform::GetRotation() const noexcept
	{
		return m_Rotation;
	}
	const glm::vec3 &Transform::GetScale() const noexcept
	{
		return m_Scale;
	}

	void Transform::SetTranslation(const glm::vec3 &translation)
	{
		m_Translation = translation;

		RecalculateTransformationMatrices();
	}
	void Transform::SetRotation(const glm::vec3 &rotation)
	{
		m_Rotation.x = rotation.x >= glm::radians(360.0f) ? rotation.x - glm::radians(360.0f) : rotation.x;
		m_Rotation.y = rotation.y >= glm::radians(360.0f) ? rotation.y - glm::radians(360.0f) : rotation.y;
		m_Rotation.z = rotation.z >= glm::radians(360.0f) ? rotation.z - glm::radians(360.0f) : rotation.z;

		RecalculateTransformationMatrices();
	}
	void Transform::SetScale(const glm::vec3& scale)
	{
		m_Scale = scale;

		RecalculateTransformationMatrices();
	}

	void Transform::Translate(const glm::vec3 & offset)
	{
		m_Translation += offset;

		RecalculateTransformationMatrices();
	}
	void Transform::Rotate(const glm::vec3 & offset)
	{
		m_Rotation += offset;

		m_Rotation.x = m_Rotation.x >= glm::radians(360.0f) ? m_Rotation.x - glm::radians(360.0f) : m_Rotation.x;
		m_Rotation.y = m_Rotation.y >= glm::radians(360.0f) ? m_Rotation.y - glm::radians(360.0f) : m_Rotation.y;
		m_Rotation.z = m_Rotation.z >= glm::radians(360.0f) ? m_Rotation.z - glm::radians(360.0f) : m_Rotation.z;

		RecalculateTransformationMatrices();
	}
	void Transform::Scale(const glm::vec3 & offset)
	{
		m_Scale *= offset;

		RecalculateTransformationMatrices();
	}

	const glm::mat4 &Transform::GetTransform() const noexcept
	{
		return m_TransformationMatrix;
	}

	void Transform::RecalculateTransformationMatrices()
	{
		m_TransformationMatrix = glm::mat4(1.0f);

		m_TransformationMatrix *= glm::translate(glm::mat4(1.0f), m_Translation);

		if (m_Rotation != glm::vec3(0.0f))
			m_TransformationMatrix *= glm::toMat4(glm::quat(m_Rotation));

		m_TransformationMatrix *= glm::scale(glm::mat4(1.0f), m_Scale);
	}
}
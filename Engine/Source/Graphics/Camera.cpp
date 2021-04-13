#include "Precompiled.h"
#include "Camera.h"

#include "Core/Event.h"
#include "Core/Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Engine
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip) :
		m_FOV(fov), m_AspectRatio(aspectRatio),
		m_NearClip(nearClip), m_FarClip(farClip)
	{
		RecalculateCameraMatrices();
	}
	EditorCamera::~EditorCamera()
	{
	}

	void EditorCamera::Rotate(const glm::vec2 &offset)
	{
		float yawSign = GetUpDirection().y < 0 ? 1.0f : -1.0f;
		m_Yaw += yawSign * offset.x * m_RotationSpeed;
		m_Pitch += -offset.y * 0.8f * m_RotationSpeed;

		RecalculateCameraMatrices();
	}
	void EditorCamera::Zoom(float offset)
	{
		m_Distance -= offset * m_ZoomSpeed;
		if (m_Distance < 0.5f)
			m_Distance = 0.5f;

		RecalculateCameraMatrices();
	}
	void EditorCamera::Move(const glm::vec2 &offset)
	{
		m_FocalPoint += -GetRightDirection() * offset.x * 0.25f * m_Distance * m_MovementSpeed;
		m_FocalPoint += GetUpDirection() * offset.y * 0.25f * m_Distance * m_MovementSpeed;

		RecalculateCameraMatrices();
	}

	void EditorCamera::OnEvent(Event &event)
	{
		if (event.type == EventType::MouseScrolled)
		{
			Zoom(event.mouse.offset.y);
		}
		if (event.type == EventType::MouseMoved)
		{
			glm::vec2 delta = m_PreviousMousePosition - event.mouse.position;
			m_PreviousMousePosition = event.mouse.position;
			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				Rotate(delta);
		}
	}
	void EditorCamera::OnResize(u32 width, u32 height)
	{
		m_AspectRatio = width / height;

		RecalculateCameraMatrices();
	}

	const glm::vec3 &EditorCamera::GetPosition() const
	{
		return m_Position;
	}
	const glm::mat4 &EditorCamera::GetProjectionViewMatrix() const
	{
		return m_ProjectionViewMatrix;
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	void EditorCamera::RecalculateCameraMatrices()
	{
		m_Position = CalculatePosition();
		glm::quat orientation = GetOrientation();

		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}
}
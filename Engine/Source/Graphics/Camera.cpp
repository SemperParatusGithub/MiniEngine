#include "Precompiled.h"
#include "Camera.h"

#include "Core/Event.h"
#include "Core/Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Engine
{
	// ----------------------------------- Orbit Camera ------------------------------------
	OrbitCamera::OrbitCamera(float fov, float aspectRatio, float nearClip, float farClip) :
		m_FOV(fov), m_AspectRatio(aspectRatio),
		m_NearClip(nearClip), m_FarClip(farClip)
	{
		RecalculateCameraMatrices();
	}
	OrbitCamera::~OrbitCamera()
	{
	}

	void OrbitCamera::Rotate(const glm::vec2 &offset)
	{
		float yawSign = GetUpDirection().y < 0 ? 1.0f : -1.0f;
		m_Yaw += yawSign * offset.x * m_RotationSpeed;
		m_Pitch += -offset.y * 0.8f * m_RotationSpeed;

		RecalculateCameraMatrices();
	}
	void OrbitCamera::Zoom(float offset)
	{
		m_Distance -= offset * m_ZoomSpeed;
		if (m_Distance < 0.5f)
			m_Distance = 0.5f;

		RecalculateCameraMatrices();
	}
	void OrbitCamera::Move(const glm::vec2 &offset)
	{
		m_FocalPoint += -GetRightDirection() * offset.x * 0.25f * m_Distance * m_MovementSpeed;
		m_FocalPoint += GetUpDirection() * offset.y * 0.25f * m_Distance * m_MovementSpeed;

		RecalculateCameraMatrices();
	}

	void OrbitCamera::OnEvent(Event &event)
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
	void OrbitCamera::OnResize(u32 width, u32 height)
	{
		m_AspectRatio = width / (height + 0.001f);

		RecalculateCameraMatrices();
	}

	const glm::vec3 &OrbitCamera::GetPosition() const
	{
		return m_Position;
	}
	const glm::mat4 &OrbitCamera::GetProjectionViewMatrix() const
	{
		return m_ProjectionViewMatrix;
	}

	glm::vec3 OrbitCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 OrbitCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::vec3 OrbitCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	glm::quat OrbitCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	void OrbitCamera::RecalculateCameraMatrices()
	{
		m_Position = CalculatePosition();
		glm::quat orientation = GetOrientation();

		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	glm::vec3 OrbitCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	// ------------------------------------ FPS Camera -------------------------------------
	FPSCamera::FPSCamera(float fov, float aspectRatio, float nearClip, float farClip) : 
		m_FOV(fov), m_AspectRatio(aspectRatio),
		m_NearClip(nearClip), m_FarClip(farClip)
	{
		RecalculateCameraMatrices();
	}
	FPSCamera::~FPSCamera()
	{
	}
	void FPSCamera::OnUpdate(float delta)
	{
		float velocity = 2.5f * delta;
		if (Input::IsKeyPressed(Key::LeftShift))
			velocity *= 4.0f;

		if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
		{
			if (Input::IsKeyPressed(Key::W))
				m_Position += m_Front * velocity;
			if (Input::IsKeyPressed(Key::S))
				m_Position -= m_Front * velocity;
			if (Input::IsKeyPressed(Key::A))
				m_Position -= m_Right * velocity;
			if (Input::IsKeyPressed(Key::D))
				m_Position += m_Right * velocity;
		}

		RecalculateCameraMatrices();
	}
	void FPSCamera::OnEvent(Event &event)
	{
		if (event.type == EventType::MouseMoved)
		{
			glm::vec2 delta = m_PreviousMousePosition - event.mouse.position;
			m_PreviousMousePosition = event.mouse.position;
			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
			{
				float yawSign = m_Up.y < 0 ? 1.0f : -1.0f;
				m_Yaw += yawSign * delta.x * 0.005f;
				m_Pitch += delta.y * 0.005f;

				if (m_Pitch >= glm::radians(90.0f))
					m_Pitch = glm::radians(89.999f);
				if (m_Pitch <= glm::radians(-90.0f))
					m_Pitch = glm::radians(-89.999f);
			}
		}

		RecalculateCameraMatrices();
	}

	void FPSCamera::OnResize(u32 width, u32 height)
	{
		m_AspectRatio = width / (height + 0.001f);

		RecalculateCameraMatrices();
	}

	const glm::mat4 &FPSCamera::GetProjectionViewMatrix() const
	{
		return m_ProjectionViewMatrix;
	}

	void FPSCamera::RecalculateCameraMatrices()
	{
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::vec3 front = {
			glm::cos(m_Yaw) * glm::cos(m_Pitch),
			glm::sin(m_Pitch),
			glm::sin(m_Yaw) * glm::cos(m_Pitch)
		};

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, worldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));

		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);

		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// ----------------------------------- Editor Camera -----------------------------------
	EditorCamera::EditorCamera() : 
		m_CameraType(CameraType::Orbit),
		m_OrbitCamera(45.0f, 1.778f, 0.1f, 1000.0f),
		m_FPSCamera(45.0f, 1.778f, 0.1f, 1000.0f)
	{
	}
	EditorCamera::EditorCamera(CameraType type) : 
		m_CameraType(type),
		m_OrbitCamera(45.0f, 1.778f, 0.1f, 1000.0f),
		m_FPSCamera(45.0f, 1.778f, 0.1f, 1000.0f)
	{
	}
	EditorCamera::~EditorCamera()
	{
	}

	void EditorCamera::SetCameraType(CameraType type)
	{
		if (type == CameraType::Orbit && m_CameraType != CameraType::Orbit)
		{
			m_CameraType = CameraType::Orbit;
			m_OrbitCamera = OrbitCamera(45.0f, m_FPSCamera.m_AspectRatio, 0.1f, 1000.0f);
		}
		else if (type == CameraType::FPS && m_CameraType != CameraType::FPS)
		{
			m_CameraType = CameraType::FPS;
			m_FPSCamera = FPSCamera(45.0f, m_OrbitCamera.m_AspectRatio, 0.1f, 1000.0f);
		}
	}
	CameraType EditorCamera::GetCameraType() const
	{
		return m_CameraType;
	}

	void EditorCamera::OnUpdate(float delta)
	{
		if (m_CameraType == CameraType::FPS)
			m_FPSCamera.OnUpdate(delta);
	}

	void EditorCamera::OnEvent(Event& event)
	{
		if (m_CameraType == CameraType::Orbit)
			m_OrbitCamera.OnEvent(event);
		else
			m_FPSCamera.OnEvent(event);
	}

	void EditorCamera::OnResize(u32 width, u32 height)
	{
		if (m_CameraType == CameraType::Orbit)
			m_OrbitCamera.OnResize(width, height);
		else
			m_FPSCamera.OnResize(width, height);
	}

	const glm::vec3& EditorCamera::GetPosition() const
	{
		if (m_CameraType == CameraType::Orbit)
			return m_OrbitCamera.m_Position;
		else
			return m_FPSCamera.m_Position;
	}

	const glm::mat4& EditorCamera::GetProjectionMatrix() const
	{
		if (m_CameraType == CameraType::Orbit)
			return m_OrbitCamera.m_ProjectionMatrix;
		else
			return m_FPSCamera.m_ProjectionMatrix;
	}
	const glm::mat4& EditorCamera::GetViewMatrix() const
	{
		if (m_CameraType == CameraType::Orbit)
			return m_OrbitCamera.m_ViewMatrix;
		else
			return m_FPSCamera.m_ViewMatrix;
	}

	const glm::mat4 &EditorCamera::GetProjectionViewMatrix() const
	{
		if (m_CameraType == CameraType::Orbit)
			return m_OrbitCamera.GetProjectionViewMatrix();
		else
			return m_FPSCamera.GetProjectionViewMatrix();
	}

	// ----------------------------------- Scene Camera -----------------------------------
	SceneCamera::SceneCamera()
	{
		RecalculateCameraMatrices();
	}

	void SceneCamera::SetBounds(float width, float height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		RecalculateCameraMatrices();
	}

	std::pair<float, float> SceneCamera::GetBounds() const
	{
		return std::pair<float, float>(m_ViewportWidth, m_ViewportHeight);
	}

	void SceneCamera::SetFOV(float FOV)
	{
		m_FOV = FOV;

		RecalculateCameraMatrices();
	}
	float SceneCamera::GetFOV() const
	{
		return m_FOV;
	}

	void SceneCamera::SetNearClip(float nearClip)
	{
		m_NearClip = nearClip;

		RecalculateCameraMatrices();
	}
	void SceneCamera::SetFarClip(float farClip)
	{
		m_FarClip = farClip;

		RecalculateCameraMatrices();
	}

	float SceneCamera::GetNearClip() const
	{
		return m_NearClip;
	}
	float SceneCamera::GetFarClip() const
	{
		return m_FarClip;
	}

	void SceneCamera::SetPosition(const glm::vec3 &position)
	{
		m_Position = position;

		RecalculateCameraMatrices();
	}
	void SceneCamera::SetRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;

		RecalculateCameraMatrices();
	}

	const glm::vec3& SceneCamera::GetPosition() const
	{
		return m_Position;
	}
	const glm::vec3& SceneCamera::GetRotation() const
	{
		return m_Rotation;
	}

	const glm::mat4& SceneCamera::GetProjectionViewMatrix() const
	{
		return m_ProjectionView;
	}

	void SceneCamera::RecalculateCameraMatrices()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;

		m_Projection = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
		m_View = glm::translate(glm::mat4(1.0f), m_Position);

		if (m_Rotation != glm::vec3(0.0f))
			m_View *= glm::toMat4(glm::quat(m_Rotation));

		m_View = glm::inverse(m_View);

		m_ProjectionView = m_Projection * m_View;
	}
}
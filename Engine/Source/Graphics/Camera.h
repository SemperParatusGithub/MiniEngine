#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>


namespace Engine
{
	struct Event;
	class EditorCamera;

	class OrbitCamera
	{
	public:
		OrbitCamera(float fov, float aspectRatio, float nearClip, float farClip);
		~OrbitCamera();

		void Rotate(const glm::vec2 &offset);
		void Zoom(float offset);
		void Move(const glm::vec2 &offset);

		void OnEvent(Event &event);
		void OnResize(u32 width, u32 height);

		const glm::vec3 &GetPosition() const;
		const glm::mat4 &GetProjectionViewMatrix() const;

	private:
		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;

		void RecalculateCameraMatrices();
		glm::vec3 CalculatePosition() const;

	private:
		float m_FOV, m_AspectRatio;		// Set in constructor
		float m_NearClip, m_FarClip;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
		glm::mat4 m_ProjectionViewMatrix;

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Yaw = 0.0f, m_Pitch = glm::radians(45.0f);

		float m_MovementSpeed = 1.0f;
		float m_RotationSpeed = 0.005f;
		float m_ZoomSpeed = 0.5f;

		glm::vec2 m_PreviousMousePosition = { 0.0f, 0.0f };

		friend class EditorCamera;
	};

	class FPSCamera
	{
	public:
		FPSCamera(float fov, float aspectRatio, float nearClip, float farClip);
		~FPSCamera();

		void OnUpdate(float delta);
		void OnEvent(Event &event);
		void OnResize(u32 width, u32 height);

		const glm::mat4 &GetProjectionViewMatrix() const;

	private:
		void RecalculateCameraMatrices();

	private:
		float m_FOV, m_AspectRatio;		// Set in constructor
		float m_NearClip, m_FarClip;

		float m_Yaw = 0.0f, m_Pitch = glm::radians(45.0f);

		glm::vec3 m_Front, m_Right, m_Up;
		glm::vec3 m_Position = { 0.0f, 2.0f, 0.0f };

		glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
		glm::mat4 m_ProjectionViewMatrix = glm::mat4(1.0f);

		glm::vec2 m_PreviousMousePosition = { 0.0f, 0.0f };

		friend class EditorCamera;
	};

	enum class CameraType
	{
		Orbit = 0, FPS
	};
	class EditorCamera
	{
	public:
		EditorCamera();
		EditorCamera(CameraType type);
		~EditorCamera();

		void SetCameraType(CameraType type);
		CameraType GetCameraType() const;

		void OnUpdate(float delta);
		void OnEvent(Event &event);
		void OnResize(u32 width, u32 height);

		const glm::vec3& GetPosition() const;

		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetViewMatrix() const;
		
		const glm::mat4 &GetProjectionViewMatrix() const;

	private:
		CameraType m_CameraType;
		OrbitCamera m_OrbitCamera;
		FPSCamera m_FPSCamera;
	};

	class SceneCamera
	{
	public:
		SceneCamera();

		void SetBounds(float width, float height);
		std::pair<float, float> GetBounds() const;

		void SetFOV(float FOV);
		float GetFOV() const;

		void SetNearClip(float nearClip);
		void SetFarClip(float farClip);

		float GetNearClip() const;
		float GetFarClip() const;

		void SetPosition(const glm::vec3& position);
		void SetRotation(const glm::vec3& rotation);

		const glm::vec3& GetPosition() const;
		const glm::vec3& GetRotation() const;

		const glm::mat4& GetProjectionViewMatrix() const;

	private:
		void RecalculateCameraMatrices();

	private:
		glm::mat4 m_Projection, m_View;
		glm::mat4 m_ProjectionView;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };

		float m_NearClip = 0.1f, m_FarClip = 1000.0f;
		float m_AspectRatio = 1.78f;

		float m_FOV = 45.0f;
		float m_Zoom = 1.0f;

		float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
	};
}
#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>


namespace Engine
{
	struct Event;

	class SceneCamera
	{

	};

	class EditorCamera
	{
	public:
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
		~EditorCamera();

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

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionViewMatrix;

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Yaw = 0.0f, m_Pitch = -25.0f;

		float m_MovementSpeed = 1.0f;
		float m_RotationSpeed = 0.005f;
		float m_ZoomSpeed = 0.5f;

		glm::vec2 m_PreviousMousePosition = { 0.0f, 0.0f };
	};
}
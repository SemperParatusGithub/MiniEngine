#pragma once
#include "Core/EngineBase.h"

#include <glm/glm.hpp>


namespace Engine
{
	class Shader
	{
	public:
		Shader(const std::string &filepath);
		~Shader();

		void LoadFromFile(const std::string &filepath);
		void LoadFromFiles(const std::string &vertexPath, const std::string &fragmentPath);

		virtual void Bind() const;

		void SetUniformMatrix4(const char *name, const glm::mat4 &matrix);
		void SetUniformFloat(const char *name, float value);
		void SetUniformFloat3(const char *name, const glm::vec3 &values);

		void SetUniformInt(const char *name, int value);

	private:
		u32 CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
		u32 TryCompileShader(u32 shaderType, const std::string &shaderSource, std::string &errorLog);

		int GetUniformLocation(const std::string& name);

	private:
		RendererID m_RendererID;
	};
}
#pragma once
#include "Core/EngineBase.h"


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

	private:
		u32 CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
		u32 TryCompileShader(u32 shaderType, const std::string &shaderSource, std::string &errorLog);

	private:
		RendererID m_RendererID;
	};
}
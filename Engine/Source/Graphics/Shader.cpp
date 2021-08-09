#include "Precompiled.h"
#include "Shader.h"

#include <fstream>
#include <string>

#include <glad/glad.h>


namespace Engine
{
    Shader::Shader(const std::string &filepath)
    {
        LoadFromFile(filepath);
    }
    Shader::~Shader()
    {
    }

    void Shader::LoadFromFile(const std::string & filepath)
    {
        ME_TRACE("Loading Shader: %s", filepath.c_str());

        std::ifstream stream(filepath);

        enum class ShaderType
        {
            None = -1, Vertex = 0, Fragment
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::None;

        while (std::getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos) 
                    type = ShaderType::Vertex;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::Fragment;
            }
            else
            {
                ss[(int) type] << line << "\n";
            }
        }

        std::string vertexSource = ss[(int) ShaderType::Vertex].str();
        std::string fragmentSource = ss[(int) ShaderType::Fragment].str();

        m_RendererID = CreateShader(vertexSource, fragmentSource);
    }
    void Shader::LoadFromFiles(const std::string &vertexPath, const std::string &fragmentPath)
    {
        ME_ASSERT(false); // Not implemented
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::SetUniformMatrix4(const char *name, const glm::mat4 &matrix)
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
    }
    void Shader::SetUniformFloat(const char *name, float value)
    {
        glUniform1f(GetUniformLocation(name), value);
    }
    void Shader::SetUniformFloat3(const char *name, const glm::vec3 &values)
    {
        glUniform3f(GetUniformLocation(name), values.x, values.y, values.z);
    }

    void Shader::SetUniformInt(const char *name, int value)
    {
        glUniform1i(GetUniformLocation(name), value);
    }

    u32 Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
    {
        u32 shaderProgram = glCreateProgram();
        std::string errorLog;

        u32 vertexProgram = TryCompileShader(GL_VERTEX_SHADER, vertexShader, errorLog);
        if (!vertexProgram)
        {
            ME_ERROR("%s", errorLog.c_str());
            ME_ASSERT(false);
        }

        errorLog.clear();

        u32 fragmentProgram = TryCompileShader(GL_FRAGMENT_SHADER, fragmentShader, errorLog);
        if (!fragmentProgram)
        {
            ME_ERROR("%s", errorLog.c_str());
            ME_ASSERT(false);
        }

        glAttachShader(shaderProgram, vertexProgram);
        glAttachShader(shaderProgram, fragmentProgram);

        glLinkProgram(shaderProgram);
        glValidateProgram(shaderProgram);

        glDeleteShader(vertexProgram);
        glDeleteShader(fragmentProgram);

        return shaderProgram;
    }
    u32 Shader::TryCompileShader(u32 shaderType, const std::string &shaderSource, std::string &errorLog)
    {
        u32 shaderID = glCreateShader(shaderType);
        const char *src = shaderSource.c_str();

        glShaderSource(shaderID, 1, &src, nullptr);
        glCompileShader(shaderID);

        int result;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

        if (!result)
        {
            int errorLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &errorLength);

            char *message = new char[errorLength * sizeof(char)];
            glGetShaderInfoLog(shaderID, errorLength, &errorLength, message);

            errorLog = "Failed to compile shader" + std::string(message);

            glDeleteShader(shaderType);
            delete[] message;
            return 0;
        }
        return shaderID;
    }

    int Shader::GetUniformLocation(const std::string& name)
    {
        int loc = glGetUniformLocation(m_RendererID, name.c_str());
        if (loc == -1){
            ME_WARN("Uniform %s not found", name.c_str());
            ME_BREAKDEBUGGER;
        }
        return loc;
    }

    ComputeShader::ComputeShader(const std::string& filepath)
    {
        std::ifstream is(filepath);
        std::stringstream ss;
        ss << is.rdbuf();
        std::string shaderSource = ss.str();

        m_RendererID = glCreateProgram();

        std::string errorLog;

        u32 computeProgram = TryCompileShader(GL_COMPUTE_SHADER, shaderSource, errorLog);
        if (!computeProgram)
        {
            ME_ERROR("%s", errorLog.c_str());
            ME_ASSERT(false);
        }

        glAttachShader(m_RendererID, computeProgram);

        glLinkProgram(m_RendererID);
        glValidateProgram(m_RendererID);

        glDeleteShader(computeProgram);
    }
}
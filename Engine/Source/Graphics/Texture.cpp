#include "Precompiled.h"
#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <glm/glm.hpp>


namespace Engine
{
	Texture::Texture() : 
		m_RendererID(0), 
		m_Width(0), m_Height(0),
		m_IsLoaded(false)
	{
	}
	Texture::Texture(const std::string &filepath, bool srgb)
	{
		Load(filepath, srgb);
	}
	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::Load(const std::string &filepath, bool srgb)
	{
		ME_INFO("Loading Texture: %s", filepath.c_str());

		int width, height;
		int channels;

		m_IsHDR = stbi_is_hdr(filepath.c_str());

		if (m_IsHDR)
		{
			float* localBuffer = stbi_loadf(filepath.c_str(), &width, &height, &channels, 0);

			if (localBuffer)
			{
				m_IsLoaded = true;
				m_Width = width;
				m_Height = height;

				glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
				glTextureStorage2D(m_RendererID, 1, GL_RGB32F, (GLsizei) width, (GLsizei) height);

				glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glTextureSubImage2D(m_RendererID, 0, 0, 0, (GLsizei)width, (GLsizei)height, GL_RGB, GL_FLOAT, localBuffer);

				stbi_image_free(localBuffer);
			}
			else {
				m_IsLoaded = false;
				ME_ERROR("Failed to load HDR Texture: %s", filepath.c_str());
			}
		}
		else {
			stbi_set_flip_vertically_on_load(true);
			stbi_uc *localBuffer = stbi_load(filepath.c_str(), &width, &height, &channels, 
				srgb ? STBI_rgb : STBI_rgb_alpha);

			if (localBuffer)
			{
				m_IsLoaded = true;
				m_Width = width;
				m_Height = height;

				if (srgb)
				{
					uint32_t levels = 1;
					while ((m_Width | m_Height) >> levels)
						levels++;
					glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
					glTextureStorage2D(m_RendererID, levels, GL_SRGB8, (GLsizei) m_Width, (GLsizei) m_Height);
					glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
					glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureSubImage2D(m_RendererID, 0, 0, 0, (GLsizei) m_Width, (GLsizei) m_Height, GL_RGB, GL_UNSIGNED_BYTE, localBuffer);
					glGenerateTextureMipmap(m_RendererID);
				}
				else
				{
					glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
					glTextureStorage2D(m_RendererID, 1, GL_RGBA8, (GLsizei) m_Width, (GLsizei) m_Height);

					glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

					glTextureSubImage2D(m_RendererID, 0, 0, 0, (GLsizei) m_Width, (GLsizei) m_Height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
				}

				stbi_image_free(localBuffer);
			}
			else {
				m_IsLoaded = false;
				ME_ERROR("Failed to load Texture: %s", filepath.c_str());
			}
		}
	}
	bool Texture::IsLoaded() const
	{
		return m_IsLoaded;
	}

	bool Texture::IsHDR() const
	{
		return m_IsHDR;
	}

	void Texture::Bind(u32 slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	u32 Texture::GetWidth() const
	{
		return m_Width;
	}
	u32 Texture::GetHeight() const
	{
		return m_Height;
	}

	RendererID Texture::GetRendererID() const
	{
		return m_RendererID;
	}


	TextureCube::TextureCube(const std::string &filepath)
	{
		ME_ASSERT(false);
	}
	TextureCube::TextureCube(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back)
	{
		stbi_set_flip_vertically_on_load(false);
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width, height, nrChannels;
		{
			unsigned char* data = stbi_load(right.c_str(), &width, &height, &nrChannels, 0);
			ME_ASSERT(data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		{
			unsigned char* data = stbi_load(left.c_str(), &width, &height, &nrChannels, 0);
			ME_ASSERT(data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		{
			unsigned char* data = stbi_load(top.c_str(), &width, &height, &nrChannels, 0);
			ME_ASSERT(data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		{
			unsigned char* data = stbi_load(bottom.c_str(), &width, &height, &nrChannels, 0);
			ME_ASSERT(data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		{
			unsigned char* data = stbi_load(front.c_str(), &width, &height, &nrChannels, 0);
			ME_ASSERT(data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		{
			unsigned char* data = stbi_load(back.c_str(), &width, &height, &nrChannels, 0);
			ME_ASSERT(data);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	TextureCube::TextureCube(u32 width, u32 height)
	{
		m_Width = width;
		m_Height = height;
		u32 levels = (uint32_t)std::floor(std::log2(glm::min(width, height))) + 1;

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, levels, GL_RGBA32F, m_Width, m_Height);
	
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void TextureCube::Bind(u32 slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	u32 TextureCube::GetMipLevelCount() const
	{
		return (uint32_t)std::floor(std::log2(glm::min(m_Width, m_Height))) + 1;
	}
}
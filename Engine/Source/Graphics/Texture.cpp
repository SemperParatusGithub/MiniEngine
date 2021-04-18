#include "Precompiled.h"
#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>


namespace Engine
{
	Texture::Texture() : 
		m_RendererID(0), 
		m_Width(0), m_Height(0),
		m_IsLoaded(false)
	{
	}
	Texture::Texture(const std::string &filepath)
	{
		Load(filepath);
	}
	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::Load(const std::string &filepath)
	{
		ME_INFO("Loading Texture: %s", filepath.c_str());

		int width, height;
		int channels;

		stbi_set_flip_vertically_on_load(true);
		stbi_uc *localBuffer = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (localBuffer)
		{
			m_IsLoaded = true;
			m_Width = width;
			m_Height = height;

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, GL_RGBA8, (GLsizei) width, (GLsizei) height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, (GLsizei) width, (GLsizei) height, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);

			stbi_image_free(localBuffer);
		}
		else {
			m_IsLoaded = false;
			ME_ERROR("Failed to load Texture: %s", filepath.c_str());
		}
	}

	void Texture::Bind(u32 slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
	u32 Texture::GetWidth() const
	{
		return u32();
	}
	u32 Texture::GetHeight() const
	{
		return u32();
	}
}
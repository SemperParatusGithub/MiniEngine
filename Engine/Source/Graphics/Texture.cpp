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
	bool Texture::IsLoaded() const
	{
		return m_IsLoaded;
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
}
#include "Precompiled.h"
#include "Framebuffer.h"

#include <glad/glad.h>


namespace Engine
{
	static GLenum FramebufferTextureFormatToInternalFormat(FramebufferTextureFormat textureFormat)
	{
		switch (textureFormat)
		{
			case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
			case FramebufferTextureFormat::RGBA32F: return GL_RGBA32F;
			case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
		}

		ME_ASSERT(false);	// unknown format
		return 0;
	}

	static GLenum FramebufferTextureFormatToDataType(FramebufferTextureFormat textureFormat)
	{
		switch (textureFormat)
		{
			case FramebufferTextureFormat::RGBA8: return GL_UNSIGNED_BYTE;
			case FramebufferTextureFormat::RGBA32F: return GL_FLOAT;
			case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_UNSIGNED_INT_24_8;
		}

		ME_ASSERT(false);	// unknown format
		return 0;
	}

	static GLenum FramebufferTextureFormatToAttachmentType(FramebufferTextureFormat textureFormat)
	{
		switch (textureFormat)
		{
			case FramebufferTextureFormat::RGBA8: return GL_COLOR_ATTACHMENT0;
			case FramebufferTextureFormat::RGBA32F: return GL_COLOR_ATTACHMENT0;
			case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
		}

		ME_ASSERT(false);	// unknown format
		return 0;
	}


	Framebuffer::Framebuffer(u32 width, u32 height) : 
		width(width), height(height)
	{
	}
	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void Framebuffer::Create()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			for (auto attachment : m_ColorAttachments)
				glDeleteTextures(1, &attachment.m_RendererID);
			for (auto attachment : m_DepthAttachments)
				glDeleteTextures(1, &attachment.m_RendererID);

			m_ColorAttachments.clear();
			m_DepthAttachments.clear();
		}

		for (auto attachment : attachments)
		{
			if (attachment.textureFormat == FramebufferTextureFormat::DEPTH24STENCIL8)
				m_DepthAttachments.emplace_back(attachment);
			else
				m_ColorAttachments.emplace_back(attachment);
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		if (!m_ColorAttachments.empty())
		{
			for (u32 i = 0; i < m_ColorAttachments.size(); i++)
			{
				GLenum textureTarget = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
				glCreateTextures(textureTarget, 1, &m_ColorAttachments[i].m_RendererID);
				glBindTexture(textureTarget, m_ColorAttachments[i].m_RendererID);

				AttachColorTexture(i);
			}
		}

		if (!m_DepthAttachments.empty())
		{
			for (u32 i = 0; i < m_DepthAttachments.size(); i++)
			{
				GLenum textureTarget = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
				glCreateTextures(textureTarget, 1, &m_DepthAttachments[i].m_RendererID);
				glBindTexture(textureTarget, m_DepthAttachments[i].m_RendererID);

				AttachDepthTexture(i);
			}
		}

		if(m_ColorAttachments.size() > 1)
			for (u32 i = 0; i < m_ColorAttachments.size(); i++)
				glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);

		ME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);	// Framebuffer incomplete

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(u32 width, u32 height)
	{
		this->width = width;
		this->height = height;

		Create();
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, width, height);
	}
	void Framebuffer::UnBind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	RendererID Framebuffer::GetColorAttachmentRendererID() const
	{
		return m_ColorAttachments[0].m_RendererID;
	}

	void Framebuffer::AttachColorTexture(u32 index)
	{
		auto& attachment = m_ColorAttachments[index];
		auto textureFormat = attachment.textureFormat;

		GLenum internalFormat = FramebufferTextureFormatToInternalFormat(textureFormat);
		GLenum dataType = FramebufferTextureFormatToDataType(textureFormat);
		GLenum textureTarget = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GLuint rendererID = attachment.m_RendererID;

		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			// Only RGBA access for now
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, dataType, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, textureTarget, rendererID, 0);
	}
	void Framebuffer::AttachDepthTexture(u32 index)
	{
		auto& attachment = m_DepthAttachments[index];
		auto textureFormat = attachment.textureFormat;

		GLenum internalFormat = FramebufferTextureFormatToInternalFormat(textureFormat);
		GLenum dataType = FramebufferTextureFormatToDataType(textureFormat);
		GLenum textureTarget = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GLenum attachmentType = FramebufferTextureFormatToAttachmentType(textureFormat);
		GLuint rendererID = attachment.m_RendererID;

		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget, rendererID, 0);
	}
}
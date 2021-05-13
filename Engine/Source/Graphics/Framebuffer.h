#pragma once
#include "Core/EngineBase.h"

#include <vector>


namespace Engine
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		RGBA8,
		DEPTH24STENCIL8
	};

	class Framebuffer;

	struct FramebufferAttachment
	{
		FramebufferAttachment(FramebufferTextureFormat format) : 
			textureFormat(format), m_RendererID(0) {}

		FramebufferTextureFormat textureFormat;

	private:
		RendererID m_RendererID;
		friend class Framebuffer;
	};

	class Framebuffer
	{
	public:
		Framebuffer(u32 width, u32 height);
		~Framebuffer();

		void Create();
		void Resize(u32 width, u32 height);

		void Bind() const;
		void UnBind() const;

		RendererID GetColorAttachmentRendererID() const;

	private:
		void AttachColorTexture(u32 index);
		void AttachDepthTexture(u32 index);

	public:
		u32 width, height;
		bool multisampled = false;
		u32 samples = 4;
		std::vector<FramebufferAttachment> attachments;

	private:
		RendererID m_RendererID = 0;

		std::vector<FramebufferAttachment> m_ColorAttachments;
		std::vector<FramebufferAttachment> m_DepthAttachments;
	};
}
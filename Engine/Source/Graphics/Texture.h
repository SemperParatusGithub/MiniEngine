#pragma once
#include "Core/EngineBase.h"


namespace Engine
{
	class Texture
	{
	public:
		Texture();
		Texture(const std::string &fileapth, bool srgb = false);
		~Texture();

		void Load(const std::string &filepath, bool srgb = false);
		bool IsLoaded() const;

		bool IsHDR() const;

		void Bind(u32 slot = 0) const;

		u32 GetWidth() const;
		u32 GetHeight() const;

		RendererID GetRendererID() const;

	private:
		RendererID m_RendererID;
		u32 m_Width, m_Height;
		bool m_IsLoaded;
		bool m_IsHDR;
	};

	class TextureCube
	{
	public:
		TextureCube(const std::string& filepath);
		TextureCube(const std::string& right, const std::string& left,
					const std::string& top, const std::string& bottom,
					const std::string& front, const std::string& back);
		TextureCube(u32 width, u32 height);
		~TextureCube() = default;

		RendererID GetRendererID() const { return m_RendererID; }

		void Bind(u32 slot = 0) const;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		u32 GetMipLevelCount() const;

	private:
		RendererID m_RendererID;
		u32 m_Width, m_Height;
	};
}
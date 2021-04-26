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

		void Bind(u32 slot = 0) const;

		u32 GetWidth() const;
		u32 GetHeight() const;

		RendererID GetRendererID() const;

	private:
		RendererID m_RendererID;
		u32 m_Width, m_Height;
		bool m_IsLoaded;
	};
}
#pragma once
#include "Core/EngineBase.h"


namespace Engine
{
	class Texture
	{
	public:
		Texture();
		Texture(const std::string &fileapth);
		~Texture();

		void Load(const std::string &filepath);

		void Bind(u32 slot = 0) const;

		u32 GetWidth() const;
		u32 GetHeight() const;

	private:
		RendererID m_RendererID;
		u32 m_Width, m_Height;
		bool m_IsLoaded;
	};
}
#pragma once
#include "Core/EngineBase.h"


namespace Engine
{
	enum class BufferUsage;
	enum class VertexFormat;
	enum class IndexFormat;

	class Tools
	{
	public:
		static int BufferUsageToOpenGLUsage(BufferUsage usage);

		static u32 IndexFormatToSize(IndexFormat format);
		static int IndexFormatToGLType(IndexFormat format);

		static u32 VertexFormatToCount(VertexFormat format);
		static u32 VertexFormatToSize(VertexFormat format);
		static int VertexFormatToType(VertexFormat format);
	};
}
#include "Precompiled.h"
#include "Tools.h"

#include "GraphicsPipeline.h"

#include <glad/glad.h>


namespace Engine
{
	int Tools::BufferUsageToOpenGLUsage(BufferUsage usage)
	{
		switch (usage)
		{
			case BufferUsage::Static:  return GL_STATIC_DRAW;
			case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
			case BufferUsage::Stream:  return GL_STREAM_DRAW;
		}

		ME_ASSERT(false);
		return 0;
	}

	u32 Tools::IndexFormatToSize(IndexFormat format)
	{
		switch (format)
		{
			case IndexFormat::Uint8:  return 1;
			case IndexFormat::Uint16: return 2;
			case IndexFormat::Uint32: return 4;
		}

		ME_ASSERT(false);
		return 0;
	}

	int Tools::IndexFormatToGLType(IndexFormat format)
	{
		switch (format)
		{
			case IndexFormat::Uint8:  return GL_UNSIGNED_BYTE;
			case IndexFormat::Uint16: return GL_UNSIGNED_SHORT;
			case IndexFormat::Uint32: return GL_UNSIGNED_INT;
		}

		ME_ASSERT(false);
		return 0;
	}

	u32 Tools::VertexFormatToCount(VertexFormat format)
	{
		switch (format)
		{
			case VertexFormat::Float1:  return 1;
			case VertexFormat::Float2:  return 2;
			case VertexFormat::Float3:  return 3;
			case VertexFormat::Float4:  return 4;
		}

		ME_ASSERT(false);
		return 0;
	}
	u32 Tools::VertexFormatToSize(VertexFormat format)
	{
		switch (format)
		{
			case VertexFormat::Float1:  return 1 * sizeof(float);
			case VertexFormat::Float2:  return 2 * sizeof(float);
			case VertexFormat::Float3:  return 3 * sizeof(float);
			case VertexFormat::Float4:  return 4 * sizeof(float);
		}

		ME_ASSERT(false);
		return 0;
	}
	int Tools::VertexFormatToType(VertexFormat format)
	{
		switch (format)
		{
			case VertexFormat::Float1:  return GL_FLOAT;
			case VertexFormat::Float2:  return GL_FLOAT;
			case VertexFormat::Float3:  return GL_FLOAT;
			case VertexFormat::Float4:  return GL_FLOAT;
		}

		ME_ASSERT(false);
		return 0;
	}
}
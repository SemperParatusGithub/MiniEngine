#include "Precompiled.h"
#include "GraphicsPipeline.h"

#include "Tools.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(push)
#pragma warning(disable : 26812)
#endif


namespace Engine
{
	// ----------------------------------- Vertex Buffer -----------------------------------
	VertexBuffer::VertexBuffer(BufferUsage usage)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, Tools::BufferUsageToOpenGLUsage(usage));
	}
	VertexBuffer::VertexBuffer(const void *vertices, u32 size, BufferUsage usage)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, Tools::BufferUsageToOpenGLUsage(usage));
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::SetData(const void *vertices, u32 size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	// ----------------------------------- Index Buffer -----------------------------------
	IndexBuffer::IndexBuffer(BufferUsage usage) : 
		m_Format(IndexFormat::Uint32), m_Count(0)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, Tools::BufferUsageToOpenGLUsage(usage));
	}
	IndexBuffer::IndexBuffer(const void *indices, u32 size, IndexFormat format, BufferUsage usage) :
		m_Format(format)
	{
		m_Count = size / Tools::IndexFormatToSize(format);
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, indices, Tools::BufferUsageToOpenGLUsage(usage));
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::SetData(const void *indices, u32 size, IndexFormat format)
	{
		m_Format = format;
		m_Count = size / Tools::IndexFormatToSize(format);

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, indices);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	int IndexBuffer::GetType()
	{
		return Tools::IndexFormatToGLType(m_Format);
	}
	int IndexBuffer::GetCount()
	{
		return static_cast<int>(m_Count);
	}

	// --------------------------------- Graphics Pipeline ---------------------------------
	GraphicsPipeline::GraphicsPipeline() : 
		m_VertexArrayRendererID(0)
	{
	}
	GraphicsPipeline::~GraphicsPipeline()
	{
	}

	void GraphicsPipeline::Create()
	{
		ME_ASSERT(vertexBuffer);
		//ME_ASSERT(indexBuffer);

		glCreateVertexArrays(1, &m_VertexArrayRendererID);
		glBindVertexArray(m_VertexArrayRendererID);

		vertexBuffer->Bind();
		if(indexBuffer)
			indexBuffer->Bind();

		ME_TRACE("---------------------------------------------------------");
		ME_TRACE("Pipeline Layout: ");

		for (std::size_t i = 0; i < layout.attributes.size(); i++)
		{
			const auto &attribute = layout.attributes[i];
			glEnableVertexAttribArray(static_cast<GLuint>(i));
			glVertexAttribPointer(
				static_cast<GLuint>(i),
				Tools::VertexFormatToCount(attribute.format),
				Tools::VertexFormatToType(attribute.format),
				attribute.normalized ? GL_TRUE : GL_FALSE,
				layout.CalculateStride(),
				reinterpret_cast<void *>((intptr_t) layout.CalculateOffset(attribute))
			);
			ME_TRACE("---------------------------------------------------------");
			ME_TRACE(" Index: %d", (int) i);
			ME_TRACE(" Count: %d", Tools::VertexFormatToCount(attribute.format));
			ME_TRACE(" Type: %s", Tools::VertexFormatToType(attribute.format) == GL_FLOAT ? "float" : "other");
			ME_TRACE(" Normalized: %d", attribute.normalized ? 1 : 0);
			ME_TRACE(" Stride: %d", layout.CalculateStride());
			ME_TRACE(" Offset: %d", layout.CalculateOffset(attribute));
		}
		ME_TRACE("---------------------------------------------------------");
	}

	void GraphicsPipeline::Bind() const
	{
		ME_ASSERT(m_VertexArrayRendererID);	// Can't bind invalid pipeline

		glBindVertexArray(m_VertexArrayRendererID);
	}

	int PipelineLayout::CalculateStride()
	{
		u32 stride = 0;

		for (std::size_t i = 0; i < attributes.size(); i++)
		{
			const auto &attribute = attributes[i];
			stride += Tools::VertexFormatToSize(attribute.format);
		}

		return static_cast<int>(stride);
	}
	int PipelineLayout::CalculateOffset(const Attribute &attribute)
	{
		u32 offset = 0;

		for (std::size_t i = 0; i < attributes.size(); i++)
		{
			const auto &currentAttribute = attributes[i];

			if (currentAttribute.name == attribute.name)
				break;

			offset += Tools::VertexFormatToSize(currentAttribute.format);
		}

		return static_cast<int>(offset);
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#pragma warning(pop)
#endif
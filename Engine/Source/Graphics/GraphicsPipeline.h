#pragma once
#include "Core/EngineBase.h"

#include "Shader.h"


namespace Engine
{
	enum class BufferUsage
	{
		Static, Dynamic, Stream
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(BufferUsage usage);
		VertexBuffer(const void *vertices, u32 size /* bytes */, BufferUsage usage);

		~VertexBuffer();

		void SetData(const void *vertices, u32 size /* bytes */);

		void Bind() const;

	private:
		RendererID m_RendererID;
	};

	enum class IndexFormat { Uint8, Uint16, Uint32 };
	class IndexBuffer
	{
	public:
		IndexBuffer(BufferUsage usage);
		IndexBuffer(const void *indices, u32 size /* bytes */, IndexFormat format, BufferUsage usage);

		~IndexBuffer();

		void SetData(const void *indices, u32 size /* bytes */, IndexFormat format);

		void Bind() const;

		int GetType();
		int GetCount();

	private:
		RendererID m_RendererID;
		IndexFormat m_Format;
		u32 m_Count;
	};

	enum class VertexFormat
	{
		Float1, Float2, Float3, Float4
	};
	struct PipelineLayout
	{
		struct Attribute
		{
			std::string name;
			VertexFormat format;
			bool normalized;
		};

		PipelineLayout() = default;
		PipelineLayout(std::initializer_list<Attribute> attributes) :
			attributes(attributes) {}

		int CalculateStride();
		int CalculateOffset(const Attribute &attribute);

		std::vector<Attribute> attributes;
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline();
		GraphicsPipeline(PipelineLayout layout, SharedPtr<Shader> shader,
			SharedPtr<VertexBuffer> vertexBuffer, SharedPtr<IndexBuffer> indexBuffer);

		~GraphicsPipeline();

		void Create(PipelineLayout layout, SharedPtr<Shader> shader,
			SharedPtr<VertexBuffer> vertexBuffer, SharedPtr<IndexBuffer> indexBuffer);

		void Bind() const;

		SharedPtr<Shader> GetShader() { return m_Shader; }

	private:
		friend class Renderer;

		PipelineLayout m_Layout;

		SharedPtr<Shader> m_Shader;

		SharedPtr<VertexBuffer> m_VertexBuffer;
		SharedPtr<IndexBuffer> m_IndexBuffer;

		RendererID m_VertexArrayRendererID;
	};
}
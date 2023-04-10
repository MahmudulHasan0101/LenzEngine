#include "Buffer.h"
#include "Shader.h"
//#include "ShaderStack.h"

namespace lenz
{
//_______________________________________________________________________________________________________________________
//                                                 V E R T E X    B U F F E R											 |
//_______________________________________________________________________________________________________________________|
	Buffer::Buffer()
		:m_Size(0), m_Stride(0)
	{
	}

	Buffer::Buffer(uint32_t type)
		: m_Type(type), m_Size(0), m_Stride(0)
	{
	}

	Buffer::~Buffer()
	{
		//Destroy();
	}

	void Buffer::Init(unsigned int size, unsigned int drawtype)
	{
		m_Size = size;
		m_Stride = 0;
		glGenBuffers(1, &m_RendererID);

		glBindBuffer(GetType(), m_RendererID);
		glBufferData(GetType(), size, nullptr, drawtype);
	}

	void Buffer::Init(unsigned int glID, unsigned int size, unsigned int stride)
	{
		this->m_RendererID = glID;
		this->m_Size = size;
		this->m_Stride = stride;
	}

	void Buffer::Init(const void* data, unsigned int size, unsigned int drawtype)
	{
		m_Size = size;
		glGenBuffers(1, &m_RendererID);

		if (data != nullptr)
			m_Stride = size;

		glBindBuffer(GetType(), m_RendererID);
		glBufferData(GetType(), size, data, drawtype);

	}
	void Buffer::Destroy()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	void Buffer::Bind() const
	{
		glBindBuffer(GetType(), m_RendererID);
	}
	void Buffer::Unbind() const
	{
		glBindBuffer(GetType(), 0);
	}

	bool Buffer::SetData(size_t offset, const void* data, size_t size)
	{
		size_t newStride = offset + size;
		bool changed = false;

		Bind();
   		glBufferSubData(GetType(), offset, size, data);
		Unbind();

		return changed;
		//m_Size+=size;
	}

	void Buffer::InvalidateData(void* data, unsigned int size)
	{
	}

	void Buffer::ClearData()
	{
	}




//_______________________________________________________________________________________________________________________
//                                                  A R R A Y    B U F F E R											 |
//_______________________________________________________________________________________________________________________|


	VertexArray::VertexArray()
	{
	}

	void VertexArray::Init()
	{
		glGenVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Init(uint32_t glID)
	{
		m_RendererID = glID;
	}


	void VertexArray::Destroy()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}


	void VertexArray::Compile(const Buffer& vertexbuffer, const Buffer& instancebuffer, const Buffer& indexbuffer, const VertexBufferLayout& vbl, const InstanceBufferLayout& ibl)
	{
		uint32_t offset = 0;
		uint32_t attributeNo = 0;

		Bind();
		vertexbuffer.Bind();
		indexbuffer.Bind();

		for (const BufferElement& vbe : vbl.elements)
		{
			glEnableVertexAttribArray(attributeNo);
			glVertexAttribPointer(attributeNo, vbe.GetCount(), vbe.GetType(), vbe.ShouldNormalize(), vbl.stride, (const void*)offset);
			offset += vbe.GetStride();
			attributeNo++;
		}

		offset = 0;
		instancebuffer.Bind();
		for (const BufferElement& ibe : ibl.elements)
		{
			glEnableVertexAttribArray(attributeNo);
			glVertexAttribPointer(attributeNo, ibe.GetCount(), ibe.GetType(), ibe.ShouldNormalize(), ibl.stride, (const void*)offset);
			glVertexAttribDivisor(attributeNo, 1);
			offset += ibe.GetStride();
			attributeNo++;
		}

		Unbind();
		vertexbuffer.Unbind();
		instancebuffer.Unbind();
		indexbuffer.Unbind();
	}


	void VertexArray::Compile(const Buffer& vertexbuffer, const Buffer& instancebuffer, const Buffer& indexbuffer, const BufferLayout& layout)
	{

		const auto& vertexelements = layout.GetVertexElements();
		const auto& instanceelements = layout.GetInstanceElements();
		uint32_t offset = 0;
		uint32_t attributeNo = 0;

		Bind();
		vertexbuffer.Bind();
		indexbuffer.Bind();

		for (const BufferElement& vbe : vertexelements)
		{
			glEnableVertexAttribArray(attributeNo);
			glVertexAttribPointer(attributeNo, vbe.GetCount(), vbe.GetType(), vbe.ShouldNormalize(), layout.GetVertexStride(), (const void*)offset);
			offset += vbe.GetStride();
			attributeNo++;
		}

		offset = 0;
		instancebuffer.Bind();
		for (const BufferElement& ibe : instanceelements)
		{
			glEnableVertexAttribArray(attributeNo);
			glVertexAttribPointer(attributeNo, ibe.GetCount(), ibe.GetType(), ibe.ShouldNormalize(), layout.GetInstanceStride(), (const void*)offset);
			glVertexAttribDivisor(attributeNo, 1);
			offset += ibe.GetStride();
			attributeNo++;
		}

		Unbind();
		vertexbuffer.Unbind();
		instancebuffer.Unbind();
		indexbuffer.Unbind();
	}

	void VertexArray::Compile(const Buffer& vertexbuffer, const Buffer& indexbuffer, const BufferLayout& layout)
	{
		Bind();
		vertexbuffer.Bind();
		indexbuffer.Bind();

		const auto& vertexelements = layout.GetVertexElements();
		uint32_t offset = 0;
		uint32_t attributeNo = 0;


		for (const BufferElement& vbe : vertexelements)
		{
			glEnableVertexAttribArray(attributeNo);
			glVertexAttribPointer(attributeNo, vbe.GetCount(), vbe.GetType(), vbe.ShouldNormalize(), layout.GetVertexStride(), (const void*)offset);
			offset += vbe.GetStride();
			attributeNo++;
		}

		Unbind();
		vertexbuffer.Unbind();
		indexbuffer.Unbind();
	}


	//void VertexArray::VertexCompile(const Buffer& vertexbuffer, const Buffer& indexbuffer, const BufferLayout& layout)
	//{
	//	Bind();
	//	vertexbuffer.Bind();
	//	indexbuffer.Bind();
	//
	//	const auto& vertexelements = layout.GetVertexElements();
	//	const auto& instanceelements = layout.GetInstanceElements();
	//	uint32_t offset = 0;
	//	uint32_t attributeNo = 0;
	//
	//	for (const BufferElement& vbe : vertexelements)
	//	{
	//		glEnableVertexAttribArray(attributeNo);
	//		glVertexAttribPointer(attributeNo, vbe.GetCount(), vbe.GetType(), vbe.IsNormalized(), layout.GetStride(), (const void*)offset);
	//		offset += vbe.GetStride();
	//		attributeNo++;
	//	}
	//
	//	Unbind();
	//	vertexbuffer.Unbind();
	//	indexbuffer.Unbind();
	//}




	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0); 
	}
}	
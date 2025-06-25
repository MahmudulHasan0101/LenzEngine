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

	Buffer::Buffer(Buffer&& buffer)
	{
		m_RendererID = buffer.m_RendererID; buffer.m_RendererID = UINT_MAX;
		m_Size = buffer.m_Size; buffer.m_Size = 0;
		m_Stride = buffer.m_Stride; buffer.m_Stride = 0;
		m_Type = buffer.m_Type; buffer.m_Type = BufferType::None;
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

		glBindBuffer(m_Type, m_RendererID);
		glBufferData(m_Type, size, nullptr, drawtype);
	}

	void Buffer::Init(unsigned int size, unsigned int type, unsigned int bufferfluidity)
	{
		m_Size = size;
		m_Type = type;
		m_Stride = 0;

		glGenBuffers(1, &m_RendererID);

		glBindBuffer(m_Type, m_RendererID);
		glBufferData(m_Type, size, nullptr, bufferfluidity);
	}

	void Buffer::Init(const void* data, unsigned int size, unsigned int drawtype)
	{
		m_Size = size;
		glGenBuffers(1, &m_RendererID);

		if (data != nullptr)
			m_Stride = size;

		glBindBuffer(m_Type, m_RendererID);
		glBufferData(m_Type, size, data, drawtype);

#ifdef PRESERVE_BUFFER_DATA_CHACHE
		m_Chache = std::unique_ptr<void>(data);
#endif
	}

	/*
	void Buffer::Init(const void* data, unsigned int size, const uint32_t type, unsigned int bufferfluidity)
	{
		m_Size = size;
		m_Type = type;
		glGenBuffers(1, &m_RendererID);

		if (data != nullptr)
			m_Stride = size;

		glBindBuffer(GetType(), m_RendererID);
		glBufferData(GetType(), size, data, drawtype);

#ifdef PRESERVE_BUFFER_DATA_CHACHE
		m_Chache = std::unique_ptr<void>(data);
#endif
	}
	*/

	void Buffer::Destroy()
	{
		if (m_RendererID != UINT_MAX)
		{
			glDeleteBuffers(1, &m_RendererID);
		}
	}

	void Buffer::Bind() const
	{
		glBindBuffer(GetType(), m_RendererID);
	}
	void Buffer::Unbind() const
	{
		glBindBuffer(GetType(), 0);
	}

	void Buffer::SetData(size_t offset, const void* data, size_t size)
	{
		Bind();
   		glBufferSubData(GetType(), offset, size, data);
		Unbind();
	}

	void Buffer::InvalidateData(void* data, unsigned int size)
	{
	}

	void Buffer::ClearData()
	{
		Bind();
		glBufferSubData(GetType(), 0, m_Stride, 0);
		Unbind();
	}




//_______________________________________________________________________________________________________________________
//                                                  A R R A Y    B U F F E R											 |
//_______________________________________________________________________________________________________________________|


	ArrayBuffer::ArrayBuffer()
	{
	}

	void ArrayBuffer::Init()
	{
		glGenVertexArrays(1, &m_RendererID);
	}

	void ArrayBuffer::Init(uint32_t glID)
	{
		m_RendererID = glID;
	}


	void ArrayBuffer::Destroy()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}


	
	void CompileSegment(Buffer* buffer, BufferLayout* bufferlayout, unsigned int& attributeNoRef)
	{
		unsigned int offset = 0;
		if (buffer != nullptr && buffer->GetSize() > 0)
		{
			buffer->Bind();

			if (bufferlayout != nullptr)
			{
				for (const BufferElement& element : bufferlayout->elements)
				{
					uint32_t subsize = element.GetStride() / element.GetSubdivideNumber();
					uint32_t subcount = element.GetCount() / element.GetSubdivideNumber();

					for (int i = 0; i < element.GetSubdivideNumber(); i++)
					{
						glEnableVertexAttribArray(attributeNoRef);

						glVertexAttribPointer(
							attributeNoRef,
							subcount,
							element.GetType(),
							element.ShouldNormalize(),
							bufferlayout->stride,
							(const void*)offset
						);

						glVertexAttribDivisor(attributeNoRef, bufferlayout->atribDivisor);

						offset += subsize;
						attributeNoRef++;
					}
				}
			}
		}
	}


	void ArrayBuffer::Compile(Buffer* vertexbuffer, Buffer* instancebuffer, Buffer* indexbuffer,
		BufferLayout* vertexbufferlayout, BufferLayout* instancebufferlayout, BufferLayout* indexbufferlayout)
	{
		unsigned int attributeNo = 0;

		Bind();
		CompileSegment(vertexbuffer, vertexbufferlayout, attributeNo);
		CompileSegment(indexbuffer, indexbufferlayout, attributeNo);
		CompileSegment(instancebuffer, instancebufferlayout, attributeNo);

		Unbind();
		if(vertexbuffer) vertexbuffer->Unbind();
		if(instancebuffer) instancebuffer->Unbind();
		if(indexbuffer) indexbuffer->Unbind();
	}

	void ArrayBuffer::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void ArrayBuffer::Unbind() const
	{
		glBindVertexArray(0); 
	}
}	
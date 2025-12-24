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
	}

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
	//                                               B U F F E R  S T O R A G E      										 |
	//_______________________________________________________________________________________________________________________|



	BufferStorage::BufferStorage() {}

	BufferStorage::BufferStorage(uint32_t type)
		: m_Type(type) {
	}

	BufferStorage::BufferStorage(BufferStorage&& other) noexcept
	{
		*this = std::move(other);
	}

	BufferStorage& BufferStorage::operator=(BufferStorage&& other) noexcept
	{
		if (this != &other)
		{
			Destroy();

			m_RendererID = other.m_RendererID;
			m_Type = other.m_Type;
			m_Size = other.m_Size;
			m_MappedPtr = other.m_MappedPtr;
			m_StorageFlags = other.m_StorageFlags;
			m_MapFlags = other.m_MapFlags;

			other.m_RendererID = UINT_MAX;
			other.m_MappedPtr = nullptr;
			other.m_Size = 0;
		}
		return *this;
	}

	BufferStorage::~BufferStorage()
	{
		Destroy();
	}

	void BufferStorage::Init(size_t size,
		unsigned int type,
		unsigned int storageFlags,
		unsigned int mapFlags)
	{
		m_Size = size;
		m_Type = type;
		m_StorageFlags = storageFlags;
		m_MapFlags = mapFlags;

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(m_Type, m_RendererID);

		glBufferStorage(m_Type, size, nullptr, storageFlags);

		m_MappedPtr = glMapBufferRange(
			m_Type,
			0,
			size,
			mapFlags
		);

		glBindBuffer(m_Type, 0);

		// Safety check
		if (!m_MappedPtr)
		{
			Destroy();
			throw std::runtime_error("Persistent buffer mapping failed");
		}
	}

	void BufferStorage::InvalidateMapPtr(unsigned int mapFlags)
	{
		glBindBuffer(m_Type, m_RendererID);

		m_MappedPtr = glMapBufferRange(
			m_Type,
			0,
			m_Size,
			mapFlags
		);
	}

	void BufferStorage::Destroy()
	{
		if (m_RendererID != UINT_MAX)
		{
			glBindBuffer(m_Type, m_RendererID);

			if (m_MappedPtr)
			{
				glUnmapBuffer(m_Type);
				m_MappedPtr = nullptr;
			}

			glDeleteBuffers(1, &m_RendererID);
			m_RendererID = UINT_MAX;
		}
	}

	void BufferStorage::Bind() const
	{
		glBindBuffer(m_Type, m_RendererID);
	}

	void BufferStorage::Unbind() const
	{
		glBindBuffer(m_Type, 0);
	}

	void BufferStorage::Write(size_t offset, const void* data, size_t size)
	{
		std::memcpy(static_cast<char*>(m_MappedPtr) + offset, data, size);
	}

	void BufferStorage::Flush(size_t offset, size_t size)
	{
		if ((m_MapFlags & GL_MAP_COHERENT_BIT) == 0)
		{
			glBindBuffer(m_Type, m_RendererID);
			glFlushMappedBufferRange(m_Type, offset, size);
			glBindBuffer(m_Type, 0);
		}
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

	void ArrayBuffer::Compile(Buffer* vertexBuffer, Buffer* indexBuffer, BufferStorage* instanceBufferStorage, BufferLayout* vertexBufferLayout, size_t storage_offset, size_t view_size)
	{
		unsigned int attribNo = 0;
		Bind();
		CompileSegment(vertexBuffer, vertexBufferLayout, attribNo);
		glVertexArrayElementBuffer(m_RendererID, indexBuffer->GetRendererID());
		glBindBufferRange(
			GL_SHADER_STORAGE_BUFFER,
			0,
			instanceBufferStorage->GetRendererID(),
			storage_offset,
			view_size
		);
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
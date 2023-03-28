#pragma once
#include"Lenz/Core.h"

#include"Lenz/Utils/UUID.h"
#include"Defines.h"

namespace lenz
{
	//enum class BufferType
	//{
	//	None, 
	//	VertexBuffer = GL_ARRAY_BUFFER, 
	//	IndexBuffer = GL_ELEMENT_ARRAY_BUFFER,
	//	InstanceBuffer = GL_ARRAY_BUFFER
	//};

	class Buffer
	{
	public:
		Buffer();
		Buffer(const uint32_t type);
		~Buffer();
		void Destroy();
		void Init(unsigned int size, unsigned int bufferfluidity = BufferFluidity::Dynamic);
		void Init(const void* data, unsigned int size, unsigned int bufferfluidity = BufferFluidity::Dynamic);
		void Init(unsigned int glID, unsigned int size, unsigned int stride);

		template<typename T>
		void Init(std::initializer_list<T> list, unsigned int drawtype = BufferFluidity::Dynamic) {	Init((void*)list.begin(), list.size(), drawtype); }

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetType() const { return m_Type; }
		inline unsigned int GetSize() const { return m_Size; }
		inline unsigned int GetStride() const { return m_Stride; }
		inline unsigned int GetRendererID() const { return m_RendererID; }


		void InvalidateData(void* data, unsigned int size);
		void ClearData();
		void SetData(unsigned int offset, const void* data, unsigned int size);

		inline void operator = (Buffer&& buffer)
		{
			m_RendererID = buffer.m_RendererID; buffer.m_RendererID = UINT_MAX;
			m_Size = buffer.m_Size; buffer.m_Size = 0;
			m_Stride = buffer.m_Stride; buffer.m_Stride = 0;
			m_Type = buffer.m_Type; buffer.m_Type = BufferType::None;
		}

	protected:
		unsigned int m_RendererID;
		unsigned int m_Size;
		unsigned int m_Stride;
		uint32_t m_Type;
	};


	class BlockBuffer : public Buffer
	{
	public:
		BlockBuffer(const uint32_t type)
			: Buffer(type)
		{
		}

		template<typename T>
		void AddBlock()
		{
			m_blocks[GetClassUUID<T>()] = m_slideStride;
			m_slideStride += sizeof(T);
		}

		template<typename ... Args>
		void AddBlocks()
		{
			(AddBlock<Args>(), ...);
		}


		void Write(uint32_t slideNo, void* data, uint32_t size, uint32_t classUUID)
		{
			LZ_CORE_ASSERT((m_blocks.find(classUUID) != m_blocks.end()), "This block does not exists");

			uint32_t offset = (m_slideStride * slideNo) + m_blocks[classUUID];
			SetData(offset, data, size);
		}

		template<typename T> 
		void Write(uint32_t slideNo, void* data)
		{
			Write(slideNo, data, sizeof(T), GetClassUUID<T>());
		}

		template<typename ... Args>
		void WriteSlide(uint32_t slideNo, const std::tuple<Args&...>& tp)
		{
			// Converting the tuple to arguments:
			std::apply(
				[&](const auto&... args)
				{
					this->Write<Args...>(slideNo, std::cref(args)...);
				},
				tp);
		}

		inline uint32_t GetWidth() { return m_slideStride; }
		inline uint32_t GetHeight() { return m_Size / m_slideStride; }

	private:
		std::unordered_map<uint32_t, uint32_t> m_blocks;
		uint32_t m_slideStride = 0;
	};


	class BufferElement
	{
	public:
		BufferElement()
		{
		}

		BufferElement(uint32_t type, uint32_t count, uint8_t normalize)
			: m_type(type), m_count(count), m_normalize(normalize)
		{
			m_stride = GetSizeOfType(type) * count;
		}

		static unsigned int GetSizeOfType(unsigned int type)
		{
			switch (type)
			{
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
			};
			LZ_ASSERT(false, "Cannot get the type you want");
			return 0;
		}

		inline uint32_t GetType() const { return m_type; }
		inline uint32_t GetCount() const { return m_count; }
		inline uint32_t GetStride() const { return m_stride; }
		inline uint8_t ShouldNormalize() const { return m_normalize; }

	private:
		uint32_t m_type;
		uint32_t m_count;
		uint8_t m_normalize;
		uint32_t m_stride;

	};


	class BufferLayout
	{
	public:
		BufferLayout()
			: m_VertexStride(0), m_InstanceStride(0)
		{}
		

		void AddVertexElement(BufferElement& ve)
		{
			m_VertexElements.push_back(ve);
			m_VertexStride += ve.GetStride();
		}

		void AddVertexElement(uint32_t type, uint32_t count, uint8_t normallize = GL_FALSE)
		{
			m_VertexElements.push_back(BufferElement(type, count, normallize));
			m_VertexStride += BufferElement::GetSizeOfType(type) * count;
		}
		
		void AddInstanceElement(uint32_t type, uint32_t count, uint8_t normallize = GL_FALSE)
		{
			m_InstanceElements.push_back(BufferElement(type, count, normallize));
			m_InstanceStride += BufferElement::GetSizeOfType(type) * count;
		}
		
		void RemoveVertexElement(uint32_t index)
		{
			m_VertexStride -= m_VertexElements[index].GetStride();
			m_VertexElements.erase(m_VertexElements.begin() + index);
		}
		
		void RemoveInstanceElement(uint32_t index)
		{
			m_InstanceStride -= m_VertexElements[index].GetStride();
			m_InstanceElements.erase(m_VertexElements.begin() + index);
		}

		void Join(const BufferLayout& layout)
		{
			JoinVerticies(layout); 
			JoinInstances(layout);
		}

		void JoinVerticies(const BufferLayout& layout)
		{
			for (const BufferElement elm : layout.GetVertexElements())
				m_VertexElements.push_back(elm);
			
			m_VertexStride += layout.m_VertexStride;
			//m_VertexElements.insert(m_VertexElements.end(), layout.GetVertexElements().begin(), layout.GetVertexElements().end());
		}

		void JoinInstances(const BufferLayout& layout)
		{
			for (const BufferElement elm : layout.GetInstanceElements())
				m_InstanceElements.push_back(elm);

			m_InstanceStride += layout.m_InstanceStride;
			//m_InstanceElements.insert(m_InstanceElements.end(), layout.GetInstanceElements().begin(), layout.GetInstanceElements().end());
		}

		void Override(const BufferLayout& layout)
		{
			m_VertexElements.insert(m_VertexElements.begin(), layout.GetVertexElements().begin(), layout.GetVertexElements().end());
			m_InstanceElements.insert(m_InstanceElements.begin(), layout.GetInstanceElements().begin(), layout.GetInstanceElements().end());
		}

	
		inline const std::vector<BufferElement> GetVertexElements() const { return m_VertexElements; }
		inline const std::vector<BufferElement> GetInstanceElements() const { return m_InstanceElements; }

		inline unsigned int GetStride() const { return m_VertexStride; }
		inline unsigned int GetInstanceStride() const { return m_InstanceStride; }

		inline void operator = (BufferLayout&& layout)
		{
			m_VertexElements = std::move(layout.m_VertexElements);
			m_InstanceElements = std::move(layout.m_InstanceElements);
			m_VertexStride = layout.m_VertexStride;
			m_InstanceStride = layout.m_InstanceStride;
		}

	private:
		std::vector<BufferElement> m_VertexElements, m_InstanceElements;
		unsigned int m_VertexStride, m_InstanceStride;
	};




	class VertexArray
	{
	public:
		VertexArray();
		void Destroy();


		void Init();
		void Init(uint32_t);

		void Bind() const;
		void Unbind() const;
		void Compile(const Buffer& vertexbuffer, const Buffer& instancebuffer, const Buffer& indexbuffer, const BufferLayout& layout);
		void Compile(const Buffer& vertexbuffer, const Buffer& indexbuffer, const BufferLayout& layout);

		inline unsigned int GetRendererID() const { return m_RendererID; }

		inline operator unsigned int() const { return m_RendererID; }
		inline void operator = (unsigned int rendererID) { m_RendererID = rendererID; }
		inline void operator = (VertexArray&& arr)
		{
			m_RendererID = arr.m_RendererID;
			arr.m_RendererID = UINT_MAX;
		}

	private:
		unsigned int m_RendererID;
	};


}


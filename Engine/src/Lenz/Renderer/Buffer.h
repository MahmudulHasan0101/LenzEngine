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
		Buffer(Buffer&&) = default;
		Buffer(const Buffer&) = default;
		Buffer(const uint32_t type);
		~Buffer();

		void Destroy();
		void Init(unsigned int size, unsigned int bufferfluidity = BufferFluidity::Dynamic);
		void Init(const void* data, unsigned int size, unsigned int bufferfluidity = BufferFluidity::Dynamic);
		void Init(unsigned int glID, unsigned int size, unsigned int stride);

		template<typename T>
		void Init(std::initializer_list<T> list, unsigned int drawtype = BufferFluidity::Dynamic) { Init((void*)list.begin(), list.size(), drawtype); }

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetType() const { return m_Type; }
		inline size_t GetSize() const { return m_Size; }
		inline size_t GetStride() const { return m_Stride; }
		inline unsigned int GetRendererID() const { return m_RendererID; }

		void ClearData();
		void InvalidateData(void* data, unsigned int size);
		bool SetData(size_t offset, const void* data, size_t size); // Sends true if it has to reallocate.

		inline void operator = (Buffer&& buffer)
		{
			m_RendererID = buffer.m_RendererID; buffer.m_RendererID = UINT_MAX;
			m_Size = buffer.m_Size; buffer.m_Size = 0;
			m_Stride = buffer.m_Stride; buffer.m_Stride = 0;
			m_Type = buffer.m_Type; buffer.m_Type = BufferType::None;
		}

		inline void operator = (const Buffer& buffer)
		{
			m_RendererID = buffer.m_RendererID;
			m_Size = buffer.m_Size;
			m_Stride = buffer.m_Stride; 
			m_Type = buffer.m_Type; 
		}

	protected:
		uint32_t m_RendererID;
		size_t m_Size;
		size_t m_Stride;
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

		inline size_t GetWidth() { return m_slideStride; }
		inline size_t GetHeight() { return m_Size / m_slideStride; }

	private:
		std::unordered_map<uint32_t, uint32_t> m_blocks;
		size_t m_slideStride = 0;
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


	/*
	class BufferLayout // TODO: modify with vertexbufferlayout and intancebufferlayout
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


		inline const std::vector<BufferElement> GetVertexElements() const { return m_VertexElements; }
		inline const std::vector<BufferElement> GetInstanceElements() const { return m_InstanceElements; }

		inline size_t GetVertexStride() const { return m_VertexStride; }
		inline size_t GetInstanceStride() const { return m_InstanceStride; }

		inline void operator = (BufferLayout&& layout)
		{
			m_VertexElements = std::move(layout.m_VertexElements);
			m_InstanceElements = std::move(layout.m_InstanceElements);
			m_VertexStride = layout.m_VertexStride;
			m_InstanceStride = layout.m_InstanceStride;
		}

	private:
		std::vector<BufferElement> m_VertexElements, m_InstanceElements;
		size_t m_VertexStride, m_InstanceStride;
	};

	*/


	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() {}
		VertexBufferLayout(const VertexBufferLayout&) = default;
		VertexBufferLayout(VertexBufferLayout&&) = default;

		void push_back(const BufferElement& element)
		{
			elements.push_back(element);
			stride += element.GetStride();
		}

		void push_back(VertexBufferLayout& layout)
		{
			for (BufferElement& element : layout.elements)
				elements.push_back(element);

			stride += layout.stride;
		}

		void push_back(uint32_t type, uint32_t count, uint8_t normallize = GL_FALSE)
		{
			elements.push_back(BufferElement(type, count, normallize));
			stride += BufferElement::GetSizeOfType(type) * count;
		}


		void erase(size_t index)
		{
			stride -= elements[index].GetStride();
			elements.erase(elements.begin() + index);
		}

		void operator = (VertexBufferLayout&& layout)
		{
			elements = std::move(layout.elements);
			stride = layout.stride;
			layout.stride = 0;
		}

		void operator = (const VertexBufferLayout& layout)
		{
			elements = layout.elements;
			stride = layout.stride;
		}

		std::vector<BufferElement> elements;
		size_t stride = 0;
	};

	typedef VertexBufferLayout InstanceBufferLayout;


	class BufferLayout // TODO: modify with vertexbufferlayout and intancebufferlayout
	{
	public:
		void AddVertexElement(const BufferElement& ve)
		{
			vertexbufferlayout.push_back(ve);
		}

		void AddInstanceElement(const BufferElement& ve)
		{
			instancebufferlayout.push_back(ve);
		}

		void AddVertexElement(uint32_t type, uint32_t count, uint8_t normallize = GL_FALSE)
		{
			vertexbufferlayout.push_back(type, count, normallize);
		}

		void AddInstanceElement(uint32_t type, uint32_t count, uint8_t normallize = GL_FALSE)
		{
			instancebufferlayout.push_back(type, count, normallize);
		}

		void RemoveVertexElement(uint32_t index)
		{
			vertexbufferlayout.erase(index);
		}

		void RemoveInstanceElement(uint32_t index)
		{
			instancebufferlayout.erase(index);
		}


		void JoinVerticies(VertexBufferLayout& layout)
		{
			vertexbufferlayout.push_back(layout);
		}

		void JoinInstances(InstanceBufferLayout& layout)
		{
			instancebufferlayout.push_back(layout);
		}

		inline const std::vector<BufferElement>& GetVertexElements() const { return vertexbufferlayout.elements; }
		inline const std::vector<BufferElement>& GetInstanceElements() const { return instancebufferlayout.elements; }

		inline size_t GetVertexStride() const { return vertexbufferlayout.stride; }
		inline size_t GetInstanceStride() const { return instancebufferlayout.stride; }

		inline void operator = (BufferLayout&& layout)
		{
			vertexbufferlayout = std::move(layout.vertexbufferlayout);
			instancebufferlayout = std::move(layout.instancebufferlayout);
		}

	private:
		VertexBufferLayout vertexbufferlayout;
		InstanceBufferLayout instancebufferlayout;
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

		void Compile(const Buffer&, const Buffer&, const Buffer&, const VertexBufferLayout& vbl, const InstanceBufferLayout& ibl);

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


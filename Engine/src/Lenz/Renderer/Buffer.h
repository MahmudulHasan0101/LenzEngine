#pragma once
#include"Lenz/Core.h"

#include"Lenz/Utils/UUID.h"
#include"Lenz/Utils/TemplateOffsetCounter.h"
#include"Defines.h"

namespace lenz
{

	class Buffer
	{
	public:
		Buffer();
		Buffer(const uint32_t type);
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&);
		~Buffer();

		void Destroy();
		void Init(unsigned int size, unsigned int bufferfluidity = BufferFluidity::Dynamic);
		void Init(const void* data, unsigned int size, unsigned int bufferfluidity = BufferFluidity::Dynamic);
		void Init(unsigned int size, unsigned int type, unsigned int bufferfluidity);

		template<typename T>
		void Init(std::initializer_list<T> list, unsigned int drawtype = BufferFluidity::Dynamic) { Init((void*)list.begin(), list.size(), drawtype); }

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetType() const { return m_Type; }
		inline size_t GetSize() const { return m_Size; }
		inline size_t GetStride() const { return m_Stride; }
		inline unsigned int GetRendererID() const { return m_RendererID; }
		void* GetStorageMap();

		void ClearData();
		void InvalidateData(void* data, unsigned int size);
		void SetData(size_t offset, const void* data, size_t size); // Sends true if it has to reallocate.

		inline void operator = (Buffer&& buffer)
		{
			m_RendererID = buffer.m_RendererID; buffer.m_RendererID = UINT_MAX;
			m_Size = buffer.m_Size; buffer.m_Size = 0;
			m_Stride = buffer.m_Stride; buffer.m_Stride = 0;
			m_Type = buffer.m_Type; buffer.m_Type = BufferType::None;
		}
		
		inline void operator = (const Buffer& buffer) = delete;

	protected:
		uint32_t m_RendererID;
		size_t m_Size = 0;
		size_t m_Stride = 0;
		uint32_t m_Type;
	};

	class BufferStorage
	{
	public:
		BufferStorage();
		BufferStorage(uint32_t type);
		BufferStorage(const BufferStorage&) = delete;
		BufferStorage& operator=(const BufferStorage&) = delete;
		BufferStorage(BufferStorage&&) noexcept;
		BufferStorage& operator=(BufferStorage&&) noexcept;
		~BufferStorage();

		void Init(size_t size,
			unsigned int type,
			unsigned int storageFlags,
			unsigned int mapFlags);

		void Destroy();

		void Bind() const;
		void Unbind() const;

		void* GetMappedPtr() const { return m_MappedPtr; }
		size_t GetSize() const { return m_Size; }
		uint32_t GetType() const { return m_Type; }
		uint32_t GetRendererID() const { return m_RendererID; }
		void InvalidateMapPtr(unsigned int mapFlags);
		void Write(size_t offset, const void* data, size_t size);

		// Only needed if NOT coherent
		void Flush(size_t offset, size_t size);

	private:
		uint32_t   m_RendererID = UINT_MAX;
		uint32_t   m_Type = 0;
		size_t     m_Size = 0;

		void* m_MappedPtr = nullptr;
		GLbitfield m_StorageFlags = 0;
		GLbitfield m_MapFlags = 0;
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

		BufferElement(uint32_t type, uint32_t count, uint8_t normalize, uint32_t subdivide = 1)
			: m_type(type), m_count(count), m_normalize(normalize), m_subdivide(subdivide)
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
		inline uint32_t GetSubdivideNumber() const { return m_subdivide; }

	private:
		uint32_t m_type;
		uint32_t m_count;
		uint8_t m_normalize;
		uint32_t m_stride;
		uint32_t m_subdivide;

	};



	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const BufferLayout&) = delete;
		BufferLayout(BufferLayout&& bufferlayout)
		{
			elements = std::move(bufferlayout.elements);
			stride = std::move(bufferlayout.stride);
		}

		void push_back(const BufferElement& element)
		{
			elements.push_back(element);
			stride += element.GetStride();
		}

		void push_back(BufferLayout& layout)
		{
			for (BufferElement& element : layout.elements)
				elements.push_back(element);

			stride += layout.stride;
		}

		void push_back(uint32_t type, uint32_t count, uint8_t normallize = GL_FALSE, uint32_t subdivide = 1)
		{
			elements.emplace_back(type, count, normallize, subdivide);
			stride += BufferElement::GetSizeOfType(type) * count;
		}

		void erase(size_t index)
		{
			stride -= elements[index].GetStride();
			elements.erase(elements.begin() + index);
		}

		void setAttribDivisor(unsigned int divisor) { atribDivisor = divisor; }

		void operator = (BufferLayout&& layout)
		{
			elements = std::move(layout.elements);
			stride = layout.stride;
			layout.stride = 0;
		}
		
		void operator = (const BufferLayout& layout)
		{
			elements = layout.elements;
			stride = layout.stride;
		}

		std::vector<BufferElement> elements;
		size_t stride = 0;
		unsigned int atribDivisor = 0;
	};



	class ArrayBuffer
	{
	public:
		ArrayBuffer();
		ArrayBuffer(const ArrayBuffer&) = default;
		ArrayBuffer(ArrayBuffer&&) = default;
		void Destroy();


		void Init();
		void Init(uint32_t id);

		void Bind() const;
		void Unbind() const;

		// Pass nullptr to avoid compiling a sepcific buffer
		void Compile(Buffer* verticies, Buffer* instancebuffer, Buffer* indexbuffer,
				BufferLayout* vertexbufferlayout, BufferLayout* instancebufferlayout, BufferLayout* indexbufferlayout);

		void Compile(Buffer* vertexBuffer, Buffer* indexBuffer, BufferStorage* instanceBufferStorage,
			BufferLayout* vertexBufferLayout, size_t storage_offset, size_t view_size);


		inline unsigned int GetRendererID() const { return m_RendererID; }

		inline operator unsigned int() const { return m_RendererID; }
		inline void operator = (unsigned int rendererID) { m_RendererID = rendererID; }
		inline void operator = (ArrayBuffer&& arr)
		{
			m_RendererID = arr.m_RendererID;
			arr.m_RendererID = UINT_MAX;
		}

	private:
		unsigned int m_RendererID;
	};


}


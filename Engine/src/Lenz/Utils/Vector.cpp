#include"Vector.h"

namespace lenz
{
	VoidVector::VoidVector()
		: m_typesize(0), m_size(0), m_stride(0)
	{
	}

	VoidVector::VoidVector(uint32_t typesize, uint32_t startupSize)
		: m_typesize(typesize), m_size(0), m_stride(0)
	{
		Init(typesize, startupSize);
	}

	VoidVector::~VoidVector()
	{
		Delete();
	}

	void VoidVector::Init(uint32_t typesize, uint32_t startupSize)
	{
		m_typesize = typesize;
		m_size = m_typesize * startupSize;
		m_memory = (char*)malloc(m_size);
	}

	void VoidVector::Delete()
	{
		free(m_memory);
	}

	void VoidVector::Push(void* data)
	{
		LZ_CORE_ASSERT((m_memory != nullptr), "This class is not intellized!");

		memcpy(&m_memory[m_stride / sizeof(char)], data, m_typesize);
		m_stride += m_typesize;

		if (m_stride >= m_size)
			Resize();
	}

	void VoidVector::Pop()
	{
		LZ_CORE_ASSERT(m_stride <= 0, "this vector is already empty!");

		memcpy(&m_memory[(m_stride-m_typesize) / sizeof(char)], nullptr, m_typesize);
		m_stride -= m_typesize;
	}

	void VoidVector::Replace(uint32_t index, void* data)
	{
		LZ_CORE_ASSERT(index > m_stride / m_typesize, "element doesn't exists!");

		memcpy(&m_memory[(index * m_typesize) / sizeof(char)], data, m_typesize);
	}

	void VoidVector::Replace(uint32_t index1, uint32_t index2)
	{
		uint32_t oneIndex = (index1 * m_typesize) / sizeof(char);
		uint32_t twoIndex = (index2 * m_typesize) / sizeof(char);

		LZ_CORE_ASSERT(oneIndex > m_stride / sizeof(char), "element does't exists!");
		LZ_CORE_ASSERT(twoIndex > m_stride / sizeof(char), "element does't exists!");

		void* one = &m_memory[oneIndex];
		void* tow = &m_memory[twoIndex];

		memcpy(one, tow, m_typesize);
		memcpy(tow, nullptr, m_typesize);
	}

	void VoidVector::Resize(float ratio)
	{
		void* newMemory = malloc(m_size * ratio);
		memcpy(newMemory, m_memory, m_size);
		delete(m_memory);
		m_memory = (char*)newMemory;
		m_size *= 2;
	}








	VoidArray::VoidArray()
		: m_size(0), m_stride(0)
	{

	}

	VoidArray::VoidArray(void* data, uint32_t allocationsize)
	{
		Init(data, allocationsize);
	}


	VoidArray::VoidArray(uint32_t allocationsize)
		: m_size(allocationsize), m_stride(0)
	{
		Init(allocationsize);
	}

	VoidArray::~VoidArray()
	{
		//Delete();
	}



	void VoidArray::Init(VoidArray&& varr)
	{
		m_size = varr.m_size;
		m_stride = varr.m_stride;
		m_memory = varr.m_memory;

		varr.m_memory = nullptr;
	}

	void VoidArray::Init(void* data, uint32_t allocationsize)
	{
		m_memory = (char*)malloc(allocationsize);
		memcpy(m_memory, data, allocationsize);
		m_size = allocationsize;
		m_stride = allocationsize;
	}

	void VoidArray::Init(uint32_t allocationsize)
	{
		m_size = allocationsize;
		m_stride = 0;
		m_memory = (char*)malloc(allocationsize);
	}

	void VoidArray::Delete()
	{
		delete[](m_memory);
	}

	void VoidArray::RemoveDataPointer()
	{
		m_memory = 0;
	}


	void VoidArray::Remove(uint32_t offset, uint32_t size)
	{
		memcpy(&m_memory[offset / sizeof(char)], nullptr, size);
		m_stride -= size;
	}

	void VoidArray::Clear()
	{
		Remove(0, m_stride);
	}


	void VoidArray::Write(uint32_t offset, void* data, uint32_t size)
	{
		memcpy(&m_memory[offset / sizeof(char)], data, size);

		uint32_t newsize = offset + size;
		if (newsize > m_stride)
			m_stride = newsize;
	}

	void VoidArray::Push(void* data, uint32_t size)
	{
		memcpy((void*)&m_memory[m_stride / sizeof(char)], data, size);
		m_stride += size;
	}

	void VoidArray::Map(char* data, uint32_t size, uint32_t offset, uint32_t elementSize, uint32_t blockSize)
	{
		LZ_CORE_ASSERT(elementSize + offset > blockSize, "element exceed the data block");
		
		for (uint32_t u = 0; u < size; u++)
		{
			uint32_t storeOffset = (blockSize * u + offset) / sizeof(char),
				     dataOffset = (elementSize * u) / sizeof(char);

			memcpy((void*)&((char*)m_memory)[storeOffset], (void*)&data[dataOffset], elementSize);
		}

		m_stride += size * elementSize;
	}

	void VoidArray::Grow(float ratio, uint32_t growthreshold)
	{
		void* newmem = malloc((m_size + growthreshold) * ratio);
		memcpy(newmem, m_memory, m_stride);
		delete(m_memory);
		m_memory = (char*)newmem;
	}

	void VoidArray::ShrinkToFit()
	{
		Grow(m_stride / m_size);
	}







	Array::Array()
		: m_size(0), m_stride(0), m_typesize(0)
	{

	}

	Array::Array(uint32_t typesize, uint32_t count)
		: m_stride(0), m_typesize(typesize), m_size(0)
	{
		Init(typesize, count);
	}

	Array::~Array()
	{
		Delete();
	}

	void Array::Init(uint32_t typesize, uint32_t count)
	{
		m_size = m_typesize * count;
		m_memory = (char*)malloc(m_size);
	}

	void Array::Delete()
	{
		free(m_memory);
	}

	void Array::Remove(uint32_t index)
	{
		memcpy(&m_memory[index*m_typesize / sizeof(char)], nullptr, m_typesize);
		m_stride -= m_typesize;
	}

	void Array::Add(uint32_t index, void* data)
	{
		memcpy(&m_memory[index * m_typesize / sizeof(char)], data, m_typesize);
		m_stride += m_typesize;
	}

}
#pragma once
#include"Lenz/Core.h"
#include <cassert>
#include <algorithm> // for std::max
#include <utility>   // for std::forward


namespace lenz
{
	template <typename T>
	class Vector2D {
	public:
		using size_type = unsigned int;

		Vector2D(size_type rows = 0, size_type cols = 0)
			: m_rows(rows), m_cols(cols), m_data(rows* cols) {}

		void resize(size_type rows, size_type cols) {
			if (rows == m_rows && cols == m_cols) return;
			std::vector<T> new_data(rows * cols);

			size_type min_rows = std::min(rows, m_rows);
			size_type min_cols = std::min(cols, m_cols);

			for (size_type i = 0; i < min_rows; ++i)
				for (size_type j = 0; j < min_cols; ++j)
					new_data[i * cols + j] = std::move(m_data[i * m_cols + j]);

			m_rows = rows;
			m_cols = cols;
			m_data = std::move(new_data);
		}

		void push_back(size_type i, size_type j, const T& value) {
			ensure_capacity(i, j);
			m_data[i * m_cols + j] = value;
		}

		void emplace_back(size_type i, size_type j, T&& value) {
			ensure_capacity(i, j);
			m_data[i * m_cols + j] = std::move(value);
		}

		template<typename... Args>
		void emplace_back(size_type i, size_type j, Args&&... args) {
			ensure_capacity(i, j);
			m_data[i * m_cols + j] = T(std::forward<Args>(args)...);
		}

		// Access
		T& operator()(size_type i, size_type j) {
			assert(i < m_rows && j < m_cols);
			return m_data[i * m_cols + j];
		}

		const T& operator()(size_type i, size_type j) const {
			assert(i < m_rows && j < m_cols);
			return m_data[i * m_cols + j];
		}

		size_type rows() const { return m_rows; }
		size_type cols() const { return m_cols; }
		size_type size() const { return m_data.size(); }
		void clear() { m_data.clear(); m_rows = m_cols = 0; }

		T* data() { return m_data.data(); }
		const T* data() const { return m_data.data(); }

	private:
		size_type m_rows = 0, m_cols = 0;
		std::vector<T> m_data;

		void ensure_capacity(size_type i, size_type j) {
			size_type new_rows = std::max(m_rows, i + 1);
			size_type new_cols = std::max(m_cols, j + 1);
			if (new_rows != m_rows || new_cols != m_cols) {
				resize(new_rows, new_cols);
			}
		}
	};



	/***
	*
	* | class1 | class2 | class1 | class3 ....
	* 
	* Info: can hold data of various size and classes | just a binder of alloca and melloc | Non resizable
	* 
	***/


	class VoidArray
	{
	public:
		VoidArray();
		VoidArray(std::initializer_list<uint32_t>);
		VoidArray(uint32_t allocationsize);
		VoidArray(void* data, uint32_t allocationsize);

		~VoidArray();

		void Init(VoidArray&&);
		void Init(uint32_t allocationSize);
		void Init(void* data, uint32_t allocationSize);
		void Delete();
		void RemoveDataPointer();

		void Remove(uint32_t offset, uint32_t size);
		void Clear();
		void Write(uint32_t offset, void* data, uint32_t size);
		void Push(void* data, uint32_t size);
		void SetSize(size_t size);
		void GrowTo(size_t size);

		template<typename T, typename...Args>
		T& Push(Args&&...args)
		{
			//T& item = *(T*)m_memory[m_stride / sizeof(char)];
			//item = T(std::forward<Args>(args)...);
			T* item = new (m_memory + (m_stride / sizeof(char))) T(std::forward<Args>(args)...);

			m_stride += sizeof(T);

			return *item;
		}

		void Map(char* data, uint32_t size, uint32_t offset, uint32_t elementSize, uint32_t blockSize);

		void ShrinkToFit();


		template<typename T, typename ... Args>
		T& Write(uint32_t index, uint32_t typesize, Args&&...args)
		{
			uint32_t offset = index * typesize / sizeof(char);
			memcpy(&m_memory[offset], &T(std::forward<Args>(args)...), typesize);
			m_stride += typesize;

			return m_memory[offset];
		}

		template<typename T = void>
		size_t GetSize()
		{
			return m_size / sizeof(T);
		}

		template<>
		size_t GetSize<void>()
		{
			return m_size;
		}

		template<typename T>
		void Print()
		{
			if (m_stride == UINT_MAX || m_stride == 0)
			{
				LZ_CORE_ERROR("This VoidArray is not intellized!");
				return;
			}

			size_t newstride = m_stride / sizeof(T);
			T* data = (T*)m_memory;

			for (size_t u = 0; u < newstride - 1; u++)
			{
				std::cout << (data[u]) << ", ";
			}

			std::cout << (data[newstride-1]) << "\n";
		}

		void SetStride(size_t offset)
		{
			m_stride = offset;
		}

		void SetGrowthBias(float bias)
		{
			m_growthBias = bias;
		}

		//inline uint32_t Size() const { return m_size; }
		inline size_t GetStride() const { return m_stride; }

		template<typename T>
		inline T& Get(size_t index) { return *(T*)&m_memory[(index * sizeof(T) / sizeof(char))]; }
		inline void* Get(size_t index, size_t classSize) { return &m_memory[(index * classSize) / sizeof(char)]; }

		template<typename T = void>
		inline T* Back() { return (T*)(void*)m_memory[(m_stride-sizeof(T)) /sizeof(char)]; }

		template<>
		inline void* Back<void>() { return (void*)m_memory[m_stride/sizeof(char)]; }

		inline void* Front() { return (void*)m_memory[0]; }

		template<typename T>
		inline T& operator [] (uint32_t index) { return Get<T>(); }

		template<typename T>
		void operator = (std::initializer_list<T> arr) {
			size_t arrstride = arr.size() * sizeof(T);
			if (arrstride != m_size)
			{
				if (m_memory == nullptr)
					Delete();

				Init((void*)arr.begin(), arrstride);
			}

			else
				Write(0, (void*)arr.begin(), arrstride);
		}

		void operator = (VoidArray&& arr)
		{
			m_memory = arr.m_memory;
			arr.m_memory = 0;

			m_size = arr.m_size; 
			arr.m_size = 0;

			m_stride = arr.m_stride;
			arr.m_stride = 0;
		}

		inline void* operator [] (uint32_t index) { return &m_memory[index / (sizeof(char))]; }

		inline void* GetData() { return m_memory; }
	protected:
		char* m_memory;
		size_t m_size, m_stride;
		float m_growthBias = 1.5f;

	};




	/***
	*  | data | data | data | data ...
	*  
	* Info: Just a binder of alloca and melloc function
	***/

	class Array
	{
	public:
		Array();
		Array(uint32_t typesize, uint32_t count = 16);
		~Array();

		void Init(uint32_t typesize, uint32_t count = 16);
		void Delete();

		void Remove(uint32_t index);
		void Add(uint32_t index, void* data);


		template<typename T, typename ... Args>
		T& Add(uint32_t index, Args&&...args)
		{
			uint32_t offset = index * m_typesize / sizeof(char);
			memcpy(&m_memory[offset], &T(std::forward<Args>(args)...), m_typesize);
			m_stride += m_typesize;
			return *(T*)&m_memory[offset];
		}

		template<typename T>
		uint32_t Size()
		{
			return m_size / sizeof(T);
		}

		template<>
		uint32_t Size<void>()
		{
			return m_size;
		}

		inline uint32_t Size() const { return m_size; }
		inline uint32_t Stride() const { return m_stride; }
		inline uint32_t TypeSize() const { return m_typesize; }

		template<typename T>
		inline T& Get(uint32_t index) { return *(T*)&m_memory[(index * sizeof(T) / sizeof(char))]; }

		//template<typename T>
		//inline T& operator [] (uint32_t index) { return Get<T>(); }

		inline void* operator [] (uint32_t offset) { return &m_memory[offset / sizeof(char)]; }
		inline char* GetCharPtr(uint32_t offset) { return &m_memory[offset]; }

		inline void* Data() { return m_memory; }
	private:
		char* m_memory;
		uint32_t m_size, m_stride, m_typesize;
	};





	/***
	* Row:      1        2        3        4
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*		| class1 | class2 | class3 | class 4 ...
	*       \________/
	*		  block1   ....	
	*	   \______________________________________/
	*						   |
	*					   Void Array
	***/

	class BlockArray
	{
	public:
		BlockArray(uint32_t allocationSize)
		{
			if (allocationSize)
				Init(allocationSize);
		}

		BlockArray(std::initializer_list<uint32_t> arr)
			: m_slideStride(0)
		{
			m_voidarray.Init(arr);
		}

		void Init(uint32_t allocatoinSize)
		{
			m_voidarray.Init(allocatoinSize);
			m_slideStride = 0;
		}


		template<typename T> 
		void AddBlock()
		{
			m_blocks[reinterpret_cast<int>(typeid(T).name())] = m_slideStride;
			m_slideStride += sizeof(T);
		}

		template<typename ... Args>
		void AddBlocks()
		{
			(AddBlock<Args>(),	...);
		}


		//Helper function:

		template<typename T>
		void SetOffet(uint32_t& u, uint32_t* offsets, std::unordered_map<uint32_t, uint32_t>& blocks)
		{
			offsets[u] = blocks[reinterpret_cast<int>(typeid(T).name())];
			u++;
			std::cout << "Arg" << u << ": " << blocks[reinterpret_cast<int>(typeid(T).name())] << "\n";
		}

		template<typename T>
		void SetDataToVoidArray(uint32_t& u, uint32_t slideNo, uint32_t* offsets, const T& t)
		{
			m_voidarray.Write((m_slideStride * slideNo) + offsets[u], (void*)&t, sizeof(T));
			u++;
		}

		void Write(uint32_t slideNo, void* data, uint32_t size, uint32_t classUUID)
		{
			LZ_CORE_ASSERT((m_blocks.find(classUUID) != m_blocks.end()), "This block does not exists");

			uint32_t offset = (m_slideStride * slideNo) + m_blocks[classUUID];
			//if (m_voidarray.Size<void>() < offset + size)
			//	m_voidarray.Grow(2.5f);

			m_voidarray.Write(offset, data, size);
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

		inline void Delete() { m_voidarray.Delete(); }


		template<typename T>
		void Print()
		{
			m_voidarray.Print<T>();
		}

		inline VoidArray& GetVoidArray() { return m_voidarray; }

	private:
		std::unordered_map<uint32_t, uint32_t> m_blocks;
		VoidArray m_voidarray;
		uint32_t m_slideStride;
	};





	class LinkedArray
	{
	public:
		LinkedArray(uint32_t allocationSize)
			: m_Array(allocationSize)
		{
		}

		struct Node
		{
			uint32_t byte_objsize;
			uint32_t byte_offset;

			Node* next(char* memory) { return (Node*)memory[byte_offset + sizeof(Node) + byte_objsize]; }

			template<typename T>
			T& datareference(char* memory) { return *(T*)memory[byte_offset + sizeof(Node)]; }

			template<typename T>
			T* datapointer(char* memory) { return (T*)memory[byte_offset + sizeof(Node)]; }
		};

		template<typename T>
		struct DataNode
		{
			Node node;
			T data;
		};


		template<typename T, typename ...Args>
		T& Push(Args...args)
		{
			while (m_Array.GetStride() + sizeof(T) > m_Array.GetSize<void>())
				m_Array.GrowTo(2*m_Array.GetSize<void>() + sizeof(T));

			Node& node = m_Array.Push<Node>();
			node.byte_offset = (m_Array.GetStride() - sizeof(Node));
			node.byte_objsize = (sizeof(T));

			T& item = m_Array.Push<T>(std::forward<Args>(args)...);

			T* ptr = node.datapointer<T>((char*)m_Array.GetData());
			T& ref = node.datareference<T>((char*)m_Array.GetData());

			//node.data = m_Array.Back<T>();
			//node.next = m_Array.Back<void>();

			return item;
		}

		template<typename T>
		T& Get(uint32_t index)
		{
			return *(T*)m_DataPointers[index];
		}

		class Iterator
		{
		public:
			Iterator(Node* ptr) :node(ptr) {}

			void Next(char* memory) 
			{
				node = (Node*)node->next(memory);
			}
			bool operator!=(const Iterator& other) const { return node != other.node; }
			bool operator!=(void* other) const { return (void*)node != other; }

			bool operator==(const Iterator& other) const { return node == other.node; }
			bool operator==(void* other) const { return (void*)node == other; }

			Node* node;
		};

		Iterator Begin() { return Iterator((Node*) Front());  }

		template<typename T>
		DataNode<T>& Back() { return *m_Array.Back<DataNode<T>>(); }
		void* Back() { return m_Array.Back(); }
		void* Front() { return m_Array.Front(); }

		void* Data() { return m_Array.GetData(); }

	private:
		VoidArray m_Array;
		std::vector<void*> m_DataPointers;
	};


	class RawVector
	{
	public:
		RawVector() {}

		RawVector(size_t elementSize, size_t elementAlignment)
			: m_ElementSize(elementSize), m_Alignment(elementAlignment)
		{
			reserve(16);
		}

		~RawVector()
		{
			::operator delete(m_Data, std::align_val_t(m_Alignment));
		}

		void push_back(const void* element)
		{
			if (m_Size == m_Capacity)
				grow();

			std::memcpy(ptr(m_Size), element, m_ElementSize);
			++m_Size;
		}

		void write(size_t index, const void* src)
		{
			assert(index < m_Size);
			std::memcpy(ptr(index), src, m_ElementSize);
		}

		template<typename T>
		void push_back_move(T&& value)
		{
			static_assert(!std::is_trivially_copyable_v<T>,
				"Use push_back() for trivially copyable types");

			if (m_Size == m_Capacity)
				grow();

			void* dst = ptr(m_Size);
			new (dst) T(std::move(value));
			++m_Size;
		}

		template<typename T>
		void write_move(size_t index, T&& value)
		{
			assert(index < m_Size);

			void* dst = ptr(index);
			std::destroy_at(reinterpret_cast<T*>(dst)); 
			new (dst) T(std::move(value)); 
		}

		template<typename T>
		void push_back_auto(const T& value)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				push_back(&value);
			else
				push_back_move(value);
		}

		template<typename T, typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_Size == m_Capacity)
				grow();

			void* dst = ptr(m_Size);
			new (dst) T(std::forward<Args>(args)...);
			++m_Size;
		}

		template<typename T>
		void destroy_all()
		{
			for (size_t i = 0; i < m_Size; ++i)
				std::destroy_at(reinterpret_cast<T*>(ptr(i)));
		}


		void* operator[](size_t index)
		{
			return ptr(index);
		}

		const void* operator[](size_t index) const
		{
			return ptr(index);
		}

		size_t size() const { return m_Size; }
		size_t capacity() const { return m_Capacity; }

	private:
		void reserve(size_t newCap)
		{
			m_Capacity = newCap;
			m_Data = ::operator new(m_Capacity * m_ElementSize,
				std::align_val_t(m_Alignment));
		}

		void grow()
		{
			size_t newCap = m_Capacity * 2;
			void* newData = ::operator new(newCap * m_ElementSize,
				std::align_val_t(m_Alignment));

			std::memcpy(newData, m_Data, m_Size * m_ElementSize);
			::operator delete(m_Data, std::align_val_t(m_Alignment));

			m_Data = newData;
			m_Capacity = newCap;
		}

		void* ptr(size_t index) const
		{
			return static_cast<std::byte*>(m_Data) + index * m_ElementSize;
		}

	private:
		void* m_Data = nullptr;
		size_t m_Size = 0;
		size_t m_Capacity = 0;
		size_t m_ElementSize;
		size_t m_Alignment;
	};


}	

#pragma once
#include"Lenz/Core.h"
#include"Entity.h"
#include"Shape.h"
#include"../Utils/Vector.h"
#include <memory>
#include <any>


namespace lenz
{
	class Storage
    {
    public:
        static std::unordered_map<uint32_t, Shape> SHAPES;
        static std::vector<Texture> TEXTURES;
        static std::vector<std::string> TEXTURE_PATHS;

        static std::shared_ptr<Texture> addTexture2D(const std::string& path, uint8_t slot)
        {
            if (auto it = std::find(TEXTURE_PATHS.begin(), TEXTURE_PATHS.end(), path);
                it != TEXTURE_PATHS.end()) {
                auto index = std::distance(TEXTURE_PATHS.begin(), it);
                return std::shared_ptr<Texture>(&TEXTURES[index]);
            }

            TEXTURES.emplace_back(Texture2D(path, slot));
            TEXTURE_PATHS.emplace_back(path);
            return std::shared_ptr<Texture>(&TEXTURES.back());
        }

        /*template<typename T>
        Shape& GetShape() {
            return SHAPES.at(ComponentMeta<T>::id);
        }*/
    };

	struct DrawElementsIndirectCommand {
		GLuint count;          // index count
		GLuint instanceCount;  // 0 = skip, 1 = draw
		GLuint firstIndex;
		GLuint baseVertex;
		GLuint baseInstance;
	};

	/*
	* Manages instance data of a drawcall
	*/

	
	//template<class _instanceData>
	//class InstanceBufferView
	//{
	//public:
	//	InstanceBufferView(char* start_ptr, size_t size)
	//		: m_MemoryPtr(start_ptr)
	//	{
	//		m_RowCount = size / sizeof(_instanceData);
	//	}

	//	// Must have read flag in the pointed buffer storage
	//	template<typename T>
	//	T* Get(size_t row_no) {
	//		constexpr size_t offset = offsetof(_instanceData, T);
	//		return reinterpret_cast<T*>(m_Ptr[row_no * sizeof(_instanceData) + offset]);
	//	}

	//	// returns the point where the object is stored
	//	template<typename T>
	//	void* Write(const T& object, size_t row_no)
	//	{
	//		static_assert(std::is_member_object_pointer_v<decltype(&_instanceData::T)> == false,
	//			"T must be a member type, not a member pointer");

	//		constexpr size_t offset = offsetof(_instanceData, T);

	//		std::byte* base =
	//			reinterpret_cast<std::byte*>(m_MemoryPtr)
	//			+ row_no * sizeof(_instanceData)
	//			+ offset;

	//		*reinterpret_cast<T*>(base) = object;
	//	}

	//	// returns the point where the object is stored
	//	template<typename T>
	//	void* Write(T&& object, size_t row_no)
	//	{
	//		using U = std::remove_reference_t<T>;

	//		constexpr size_t offset = offsetof(_instanceData, U);

	//		std::byte* base =
	//			reinterpret_cast<std::byte*>(m_MemoryPtr)
	//			+ row_no * sizeof(_instanceData)
	//			+ offset;

	//		*reinterpret_cast<U*>(base) = std::forward<T>(object);
	//	}


	//	inline size_t GetRowCount() const { return m_RowCount; }
	//	inline size_t GetRowSize() const { return sizeof(_instanceData); }
	//private:
	//	char* m_MemoryPtr;
	//	size_t m_RowCount;
	//};



	template<class _instanceData>
	class InstanceBufferView
	{
	public:
		InstanceBufferView(char* start_ptr, size_t size)
			: m_MemoryPtr(start_ptr)
		{
			m_RowCount = size / sizeof(_instanceData);
		}

		// Must have read flag in the pointed buffer storage
		_instanceData* Get(size_t row_no)
		{
			assert(row_no < m_RowCount);
			return reinterpret_cast<_instanceData*>(
				m_MemoryPtr + row_no * sizeof(_instanceData)
				);
		}

		// returns the point where the object is stored
		_instanceData* Write(const _instanceData& object, size_t row_no)
		{
			assert(row_no < m_RowCount);

			_instanceData* dst = reinterpret_cast<_instanceData*>(
				m_MemoryPtr + row_no * sizeof(_instanceData)
				);

			std::memcpy(dst, &object, sizeof(_instanceData));
			return dst;
		}

		// returns the point where the object is stored
		_instanceData* Write(_instanceData&& object, size_t row_no)
		{
			assert(row_no < m_RowCount);

			_instanceData* dst = reinterpret_cast<_instanceData*>(
				m_MemoryPtr + row_no * sizeof(_instanceData)
				);

			std::memcpy(dst, &object, sizeof(_instanceData));
			return dst;
		}

		inline size_t GetRowCount() const { return m_RowCount; }
		inline size_t GetRowSize()  const { return sizeof(_instanceData); }

	private:
		char* m_MemoryPtr;
		size_t m_RowCount;
	};



	template<typename _instanceData>
	class InstanceBufferViewManager
	{
	public:
		InstanceBufferViewManager(const InstanceBufferView< _instanceData>& instanceBufferView, size_t indexcount, size_t reserve = 1000)
			: _InstanceBufferView(instanceBufferView), m_IndexCount(indexcount), m_Stride(0), m_CommandStride(0)
		{
			m_DrawCommands.Init(
				reserve,
				GL_DRAW_INDIRECT_BUFFER,
				GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT,
				GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT
			);

			ExpandIndirectCommands(reserve);
		}

		InstanceBufferViewManager(char* start_ptr, size_t view_size, size_t indexcount, size_t reserve = 1000)
			: _InstanceBufferView(start_ptr, view_size), m_IndexCount(indexcount), m_Stride(0), m_CommandStride(0)
		{
			ExpandIndirectCommands(reserve);
		}

		// Returns index of the stored enitity
		Entity AddInstance(const _instanceData& instanceData)
		{
			Entity entity;

			if (m_VacantRows.size() > 0)
			{
				entity._LocalIndex = m_VacantRows.back();
				m_VacantRows.pop_back();

				entity._DataPtr = _InstanceBufferView.Write(instanceData, entity._LocalIndex);

				DrawElementsIndirectCommand* commands = reinterpret_cast<DrawElementsIndirectCommand*>(m_DrawCommands.GetMappedPtr());
				commands[entity._LocalIndex].instanceCount = 1;

				return entity;
			}
			else {
				if (m_CommandStride < m_Stride)
				{
					ExpandIndirectCommands(100);
				}

				entity._LocalIndex = m_Stride;
				entity._DataPtr = _InstanceBufferView.Write(instanceData, entity._LocalIndex);
				m_Stride++;
				return entity;
			}
		}

		Entity AddInstance(_instanceData&& instanceData)
		{
			Entity entity;

			if (!m_VacantRows.empty())
			{
				entity._LocalIndex = m_VacantRows.back();

				m_VacantRows.pop_back();

				entity._DataPtr = _InstanceBufferView.Write(instanceData, entity._LocalIndex);

				auto* commands =
					reinterpret_cast<DrawElementsIndirectCommand*>(m_DrawCommands.GetMappedPtr());
				commands[entity._LocalIndex].instanceCount = 1;

				return entity;
			}
			else
			{
				if (m_CommandStride < m_Stride)
				{
					ExpandIndirectCommands(100);
				}

				entity._LocalIndex = m_Stride;
				entity._DataPtr = _InstanceBufferView.Write(std::move(instanceData), m_Stride);
				m_Stride++;
				return entity;
			}
		}


		void RemoveInstance(size_t entity)
		{
			DrawElementsIndirectCommand* commands = reinterpret_cast<DrawElementsIndirectCommand*>(m_DrawCommands.GetMappedPtr());
			commands[entity].instanceCount=0;
			m_VacantRows.push_back(entity);
		}

		void ExpandIndirectCommands(size_t instanceCount)
		{
			DrawElementsIndirectCommand* commands = reinterpret_cast<DrawElementsIndirectCommand*>(m_DrawCommands.GetMappedPtr());
			
			size_t size_needed = instanceCount + m_CommandStride;

			if (m_DrawCommands.GetSize() < size_needed)
			{
				LZ_CORE_WARN("Increasing indirect command buffer size, increase the reserve size given on constructor to prevent useless reallocation");

				m_DrawCommands.Init(
					size_needed,
					GL_DRAW_INDIRECT_BUFFER,
					GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT,
					GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT
				);

				commands = reinterpret_cast<DrawElementsIndirectCommand*>(m_DrawCommands.GetMappedPtr());
			}

			for (uint32_t i = 0; i < instanceCount; i++) {
				commands[i].count = m_IndexCount;
				commands[i].instanceCount = 1;
				commands[i].firstIndex = 0;
				commands[i].baseVertex = 0;
				commands[i].baseInstance = i;
			}

			m_CommandStride += instanceCount;
		}


		inline size_t GetStride() const { return m_Stride; }
		inline size_t GetInstanceCount() const { return m_Stride; }
		inline size_t GetActiveInstanceCount() const { return m_Stride - m_VacantRows.size(); }
		inline size_t GetInactiveInstanceCount() const { return m_VacantRows.size(); }
		inline size_t GetIndirectCommandCount() const { return m_CommandStride; }
		inline const BufferStorage& GetIndirectCommands() const { return m_DrawCommands; }

		InstanceBufferView<_instanceData> _InstanceBufferView;
	private:
		BufferStorage m_DrawCommands;
		size_t m_IndexCount;
		size_t m_CommandStride;
		size_t m_Stride;
		std::vector<size_t> m_VacantRows;
	};



	class AttributeStorage
	{
	public:
		AttributeStorage()
		{
			
		}

		template<typename T>
		T* GetAttribute(size_t index)
		{
			auto& any_vec = _Storage[GetClassUUID<T>()];
			if (!any_vec.has_value())
				return nullptr;

			auto& vector = std::any_cast<std::vector<T>&>(any_vec);

			return &vector[index];
		}


		template<typename T>
		T* SetAttribute(size_t index, const T& t)
		{
			auto& any_vec = _Storage[GetClassUUID<T>()];
			if (!any_vec.has_value())
				return nullptr;

			auto& vector = std::any_cast<std::vector<T>&>(any_vec);

			vector[index] = t;
			return &vector[index];
		}

		template<typename T>
		T* SetAttribute(size_t index, T&& t)
		{
			auto& any_vec = _Storage[GetClassUUID<T>()];
			if (!any_vec.has_value())
				return nullptr;

			auto& vector = std::any_cast<std::vector<T>&>(any_vec);

			vector[index] = std::move(t);
			return &vector[index];
		}


		//returns index where it has been stored
		template<typename T>
		std::pair<size_t, T*> PushAttribute(const T& t)
		{
			auto& any_vec = _Storage[GetClassUUID<T>()];
			if (!any_vec.has_value())
				any_vec = std::vector<T>{};

			auto& vector = std::any_cast<std::vector<T>&>(any_vec);

			vector.push_back(t);
			return std::make_pair(vector.size() - 1, &vector[vector.size() - 1]);
		}

		template<typename T>
		std::pair<size_t, T*> PushAttribute(T&& t)
		{
			auto& any_vec = _Storage[GetClassUUID<T>()];
			if (!any_vec.has_value())
				any_vec = std::vector<T>{}; 

			auto& vector = std::any_cast<std::vector<T>&>(any_vec);

			vector.push_back(std::move(t));

			return std::make_pair(vector.size() - 1, &vector[vector.size() - 1]);
		}

		std::unordered_map<uint32_t, std::any> _Storage;
	};


	class UnitAttributeStorageManager
	{
	public:
		UnitAttributeStorageManager(AttributeStorage* ptr, size_t estimated_entity_count = 100)
			: _Storage(ptr), _Capacity(estimated_entity_count)
		{
		}

		template<typename T>
		T* GetAttribute(size_t index)
		{
			if (!_AttributeExistanceMap.contains(GetClassUUID<T>()))
				return nullptr;

			size_t i = _AttributeExistanceMap[GetClassUUID<T>()][index];

			if (i == -1)
				return nullptr;

			return _Storage->GetAttribute<T>(i);
		}

		template<typename T>
		T* SetAttribute(size_t index, const T& t)
		{
			if (!_AttributeExistanceMap.contains(GetClassUUID<T>()))
			{
				std::vector<size_t>& vec = _AttributeExistanceMap[GetClassUUID<T>()];
				vec.resize(_Capacity, static_cast<size_t>(-1));
			}

			size_t& i = _AttributeExistanceMap[GetClassUUID<T>()][index];

			if (i == -1)
			{
				auto [idx, ptr] = _Storage->PushAttribute<T>(t);
				i = idx;
				return ptr;
			}
			else
			{
				return _Storage->SetAttribute(i, t);
			}
		}


		template<typename T>
		T* SetAttributeMove(size_t index, T&& t)
		{
			if (!_AttributeExistanceMap.contains(GetClassUUID<T>()))
			{
				std::vector<size_t>& vec = _AttributeExistanceMap[GetClassUUID<T>()];
				vec.resize(_Capacity, static_cast<size_t>(-1));
			}

			size_t& i = _AttributeExistanceMap[GetClassUUID<T>()][index];

			if (i == -1)
			{
				auto [idx, ptr] = _Storage->PushAttribute<T>(std::move(t));
				i = idx;
				return ptr;
			}
			else
			{
				return _Storage->SetAttribute<T>(i, std::move(t));
			}
		}

		template<typename T>
		bool RemoveAttribute(size_t index)
		{
			if (!_AttributeExistanceMap.contains(GetClassUUID<T>()))
			{
				std::vector<size_t>& vec = _AttributeExistanceMap[GetClassUUID<T>()];
				vec.resize(_Capacity, static_cast<size_t>(-1));
			}

			size_t i = _AttributeExistanceMap[GetClassUUID<T>()][index];

			if (i == -1)
				return false;

			_Storage->GetAttribute<T>(i).~T();
			return true;
		}

		void Reserve(size_t size)
		{
			if (_Capacity >= size) return;

			_Capacity = size;

			for (auto& kv : _AttributeExistanceMap) {
				kv.second.resize(_Capacity, static_cast<size_t>(-1));
			}
		}

		AttributeStorage* _Storage;
		size_t _Capacity;
		// stores the index of the attribute for the i'th entity, -1 if dones't exist
		std::unordered_map<uint32_t, std::vector<size_t>> _AttributeExistanceMap;
	};




	//class DrawUniteSpecification
	//{
	//public:

	//private:
	//};

	template<typename _instanceData>
	class DrawUnit
	{
	public:
		DrawUnit(Shape* shape, AttributeStorage& attribute_storage, BufferStorage& render_storage, size_t storage_offset, size_t view_size, size_t entity_count_estimation = 100)
			: m_Shape(shape),
			m_InstanceManager(
				reinterpret_cast<char*>(render_storage.GetMappedPtr())+ storage_offset,
				view_size,
				shape->indexBuffer.GetStride()/sizeof(unsigned int)
			),
			m_Attributes(&attribute_storage, entity_count_estimation)

		{
			m_EntityCount = 0;
			m_MAX_ENTITY_COUNT = view_size / sizeof(_instanceData);
			m_Arraybuffer.Init();
			m_Arraybuffer.Compile(& m_Shape->vertexBuffer, & m_Shape->indexBuffer, &render_storage, & m_Shape->vertexLayout, storage_offset, view_size);
		}

		void Reserve(size_t count)
		{
			m_Attributes.Reserve(count);
			if (count > m_InstanceManager.GetIndirectCommandCount())
				m_InstanceManager.ExpandIndirectCommands(count - m_InstanceManager.GetIndirectCommandCount());
		}

		Entity AddEntity(const _instanceData& instanceData)
		{
			m_EntityCount++;
			return m_InstanceManager.AddInstance(instanceData);
		}

		void RemoveEntity(const Entity& entity)
		{
			m_EntityCount--;
			m_InstanceManager.RemoveInstance(entity._LocalIndex);
		}

		void Draw(const lenz::Shader& shader)
		{
			shader.Bind();
			m_Arraybuffer.Bind();
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_InstanceManager.GetIndirectCommands().GetRendererID());

			glMultiDrawElementsIndirect(
				GL_TRIANGLES,
				GL_UNSIGNED_INT,
				nullptr,
				m_InstanceManager.GetInstanceCount(),
				0
			);
		}

		inline UnitAttributeStorageManager& GetAttributeStorageManager() { return m_Attributes; }

	private:
		size_t m_MAX_ENTITY_COUNT;
		size_t m_EntityCount;
		UnitAttributeStorageManager m_Attributes;
		InstanceBufferViewManager<_instanceData> m_InstanceManager;
		ArrayBuffer m_Arraybuffer;
		Shape* m_Shape = nullptr;
	};




	/*
	* A Scene will create a storage buffer where all the per instance data will be stored,
	* every entity of a scene will be able to interact with each other, the entities of  a drawunit
	* must have Transformation Matrix as a instance component to enable physics simulation
	*/
	class Scene
	{
	public:
		Scene(size_t instanceStorageSize = 1024 * 1024, size_t DrawUniteCountAssumption = 8)
			: m_PerInstanceDataStorage(), m_DrawUniteCount(0), m_MaxDrawUniteCount(DrawUniteCountAssumption)
		{
			m_EachDrawUniteStorageSize = instanceStorageSize / m_MaxDrawUniteCount;
			m_AttributeStorage._Storage;

			m_PerInstanceDataStorage.Init(
				instanceStorageSize,
				BufferType::ShaderStorage,
				GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT,
				GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT
			);
		}

		// TODO: Handling when drawunite count exceeds max; Have custom size for each DrawUnit based on their needs;
		template<typename _shape, typename  _instanceData>
		DrawUnit<_instanceData> CreateDrawUnite(Shape&& shape)
		{
			const uint32_t drawcall_uuid = GetCombinedClassUUID<VOID_CLASS_BINDER<_shape>, _instanceData>();

			auto shape_result = Storage::SHAPES.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(GetClassUUID<_shape>()),
				std::forward_as_tuple(std::move(shape))
			);

			if (!shape_result.second) LZ_CORE_WARN("Shape already exist at", GetClassUUID<_shape>());

			m_DrawUniteCount++;

			return DrawUnit<_instanceData>(&shape_result.first->second, m_AttributeStorage, m_PerInstanceDataStorage, m_EachDrawUniteStorageSize * m_DrawUniteCount, m_EachDrawUniteStorageSize);
		}

	private:
		AttributeStorage m_AttributeStorage;
		BufferStorage m_PerInstanceDataStorage;
		size_t m_EachDrawUniteStorageSize;
		size_t m_DrawUniteCount;
		size_t m_MaxDrawUniteCount;
	};

};





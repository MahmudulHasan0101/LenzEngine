#pragma once
#include"Lenz/Core.h"
#include"entt.hpp"
#include"Lenz/Renderer/Defines.h"
#include"Lenz/Utils/Vector.h"
#include"Lenz/Renderer/Shader.h"
#include"Lenz/Scene/Shape.h"
#include"Lenz/Renderer/Renderer.h"
#include"Entity.h"


namespace lenz
{
	template<typename T>
	struct VOID_CLASS_BINDER
	{
		VOID_CLASS_BINDER() {};
		VOID_CLASS_BINDER(const VOID_CLASS_BINDER&) {};
		VOID_CLASS_BINDER(VOID_CLASS_BINDER&&) {};

		void operator = (const VOID_CLASS_BINDER&) {}
		void operator = (VOID_CLASS_BINDER&&) {}
	};


	class Scene;

	class Drawcall
	{
		friend class Scene;
	public:
		static std::unordered_map<std::string, Shape> SHAPES;

		Drawcall()
			: m_attributesChache(0)
		{

		}


		Drawcall(const Shape& shape, entt::registry& reg)
			: m_attributesChache(100), m_instanceBuffer(BufferType::InstanceBuffer), m_registryPtr(&reg)
		{
			m_arraybuffer.Init();
			m_instanceBuffer.Init(100);

			if (SHAPES.find(shape.name) == SHAPES.end())
				SHAPES[shape.name] = shape;

			m_shapePtr = &SHAPES[shape.name];
		}

		Drawcall(std::string& shape, entt::registry& reg)
			: m_attributesChache(100), m_instanceBuffer(BufferType::InstanceBuffer), m_registryPtr(&reg)
		{
			m_arraybuffer.Init();
			m_instanceBuffer.Init(100);

			m_shapePtr = &SHAPES.at(shape);
		}


		~Drawcall()
		{
			m_attributesChache.Delete();
			m_instanceBuffer.Destroy();
		}


		template<typename ... _comps>
		void AddAttributes()
		{
			m_attributesChache.AddBlocks<_comps...>();
			(m_instancebufferlayout.push_back(_comps::GetFrameworkType(), sizeof(_comps) / _comps::GetTypeSize(), _comps::GetFrameworkType() == GL_FLOAT ? GL_FALSE : GL_TRUE), ...);

			LZ_CORE_ASSERT((m_shapePtr->vertexBuffer.GetStride() != 0 && m_shapePtr->indexBuffer.GetStride() != 0), " A m_shapePtr must be binded to see the result of Drawcall | Try calling Update functoin before this function. ")
				m_arraybuffer.Compile(m_shapePtr->vertexBuffer, m_instanceBuffer, m_shapePtr->indexBuffer, m_shapePtr->vertexLayout, m_instancebufferlayout);

			((columnSize += sizeof(_comps)), ...);
		}



		template<typename _shape, typename ... _attribs>
		void Update(bool clear = false)
		{
			auto view = m_registryPtr->view<VOID_CLASS_BINDER<_shape>, _attribs...>();

			size_t size = view.size();
			size *= columnSize;

			if (!clear) m_attributesChache.GetVoidArray().Clear();

			if (m_attributesChache.GetVoidArray().GetSize<void>() < size)
				m_attributesChache.GetVoidArray().SetSize(size * 2);


			std::vector<std::thread> threads;			
			size_t entity_count = view.size() / std::thread::hardware_concurrency();

			for (uint32_t c = 0; c < std::thread::hardware_concurrency(); c++)
			{
				threads.emplace_back(
					[&](uint32_t numcore)
					{
						size_t u = numcore * entity_count;
						auto begin = std::next(view.begin(), numcore * entity_count);
						auto end = std::next(begin, entity_count);

						for (auto it = begin; it != end; it++)
						{
							(m_attributesChache.Write(
								u,
								&view.get<_attribs>(*it),
								sizeof(_attribs),
								GetClassUUID<_attribs>()
							), ...);

							u++;

							if (begin++ == end)
								break;
						}
					}, c);
			}

			for (std::thread& t : threads)
				t.join();
			
			bool compile = false;
			if (m_instanceBuffer.GetSize() < m_attributesChache.GetVoidArray().GetSize<void>())
			{
				m_instanceBuffer.Init(m_attributesChache.GetVoidArray().GetData(), m_attributesChache.GetVoidArray().GetSize<void>());
				compile = true;
			}

			m_instanceBuffer.SetData(0, m_attributesChache.GetVoidArray().GetData(), m_attributesChache.GetVoidArray().GetSize<void>());
			if (compile) m_arraybuffer.Compile(m_shapePtr->vertexBuffer, m_instanceBuffer, m_shapePtr->indexBuffer, m_shapePtr->vertexLayout, m_instancebufferlayout);
		}

		void Run(const Shader& shader)
		{
			Renderer::Draw(m_registryPtr->size(), m_shapePtr->indexBuffer.GetStride() / sizeof(uint32_t), m_arraybuffer, shader);
		}


		void operator = (Drawcall&& drawcall)
		{
			m_instanceBuffer = std::move(drawcall.m_instanceBuffer);
			m_attributesChache = std::move(drawcall.m_attributesChache);
			m_arraybuffer = std::move(drawcall.m_arraybuffer);
			m_instancebufferlayout = std::move(drawcall.m_instancebufferlayout);

			m_registryPtr = drawcall.m_registryPtr;
			drawcall.m_registryPtr = nullptr;

			m_shapePtr = drawcall.m_shapePtr;
			drawcall.m_shapePtr = nullptr;
		}


		template<typename _formate>
		void PrintAttributes()
		{
			m_attributesChache.Print<_formate>();
		}

	private:
		template<typename _view, typename ... _attribs>
		void UploadComponentToAttributes(_view& view, uint32_t numcore)
		{
			size_t entity_count = view.size() / std::thread::hardware_concurrency();

			size_t u = numcore * entity_count;
			auto begin = std::next(view.begin(), numcore * entity_count);
			auto end   = std::next(begin, entity_count-1);

			for (auto it = begin; it != end; it++)
			{
				(m_attributesChache.Write(
					u,
					&view.get<_attribs>(e),
					sizeof(_attribs),
					GetClassUUID<_attribs>()
				), ...);

				u++;

				if (begin++ == end)
					break;
			}
		}


		Buffer m_instanceBuffer;
		BlockArray m_attributesChache;

		VertexArray m_arraybuffer;
		InstanceBufferLayout m_instancebufferlayout;
		uint32_t columnSize = 0;

		Shape* m_shapePtr = nullptr;
		entt::registry* m_registryPtr;
	};


	template<typename _shape, typename ... _comps>
	class SceneDrawcall
	{
	public:
		SceneDrawcall(Drawcall* drawcall, Scene* scene)
			: drawcall(drawcall), scene(scene)
		{
		}

		template<typename ... _attributes>
		void Update()
		{
			drawcall->Update<_shape, _attributes ...>();
		}

		template<typename ... _attributes>
		void AddAttributes()
		{
			uint32_t oldKey = GetCombinedClassUUID<_shape, _attributes...>();
			uint32_t newKey = GetCombinedClassUUID<_shape, _attributes..., _comps...>();

			if (scene.m_drawcalls.find(newKey) != scene.m_drawcalls.end())
			{
				LZ_CORE_ERROR("Counldn't add attributes, the attributed class already exists!");
				return;
			}

			scene->m_drawcalls[newKey] = std::move(scene->m_drawcalls[oldKey]);
			scene->m_drawcalls.erase(oldKey);

			drawcall->AddAttributes<_attributes>();

			(&this) = SceneDrawcall<_shape, _comps..., _attributes...>(drawcall, scene);
		}

		Entity CreateEntity()
		{
			Entity entity = Entity(&scene->m_registry);

			entity.AddVoidComponent<VOID_CLASS_BINDER<_shape>>();
			(entity.AddVoidComponent<_comps>(), ...);

			return entity;
		}


		void Run(const Shape& shader)
		{
			drawcall->Run(shader);
		}

		//private:
		Drawcall* drawcall;
		Scene* scene;
	};

};
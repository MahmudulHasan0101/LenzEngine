#pragma once
#include"Drawcall.h"
#include"Lenz/Core.h"

#include <memory>


namespace lenz
{
	class Scene
	{
	public:
		template<typename _shape, typename ... _attributes>
		SceneDrawcall<_shape, _attributes...> CreateDrawcall(Shape&& shape)
		{
			const uint32_t drawcall_uuid = GetCombinedClassUUID<VOID_CLASS_BINDER<_shape>, _attributes...>();

			Drawcall* drawcallptr = nullptr;
			 
			auto shape_result = Storage::SHAPES.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(GetClassUUID<_shape>()),
				std::forward_as_tuple(std::move(shape))
			);

			auto drawcall_result = m_drawcalls.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(drawcall_uuid),
				std::forward_as_tuple(GetClassUUID<_shape>(), std::ref(m_registry))
			);

			if (!shape_result.second) LZ_CORE_WARN("Shape already exist at", GetClassUUID<_shape>());
			if (!drawcall_result.second) LZ_CORE_WARN("Drawcall already exist at", drawcallptr);
					
			drawcallptr = &drawcall_result.first->second;
			drawcallptr->m_shape = &shape_result.first->second;


			return SceneDrawcall<_shape, _attributes...>(drawcallptr, this);
		}

		template<typename _shape, typename ... _attributes>
		SceneDrawcall<_shape, _attributes...> GetDrawcall()
		{
			auto& drawcall = m_drawcalls[GetCombinedClassUUID<VOID_CLASS_BINDER<_shape>, _attributes...>()];

			return SceneDrawcall<_shape, _attributes...>(&drawcall, this);
		}

		entt::registry& GetRegistry()
		{
			return m_registry;
		}


	private:
		std::unordered_map<uint32_t, Drawcall> m_drawcalls;
		entt::registry m_registry;

		template<typename, typename...>
		friend class SceneDrawcall;

		friend class PhysicsSystem;
	};

};
#pragma once
#include"Drawcall.h"
#include"Lenz/Core.h"


namespace lenz
{
	class Scene
	{
	public:
		template<typename _shape, typename ... _attributes>
		SceneDrawcall<_shape, _attributes...> CreateDrawcall()
		{
			auto& drawcall = m_drawcalls[GetCombinedClassUUID<_shape, _attributes...>()];

			if (drawcall.m_shapePtr == nullptr)
			{
				drawcall = Drawcall(_shape(), m_registry);
				drawcall.AddAttributes<_attributes...>();
			}

			return SceneDrawcall<_shape, _attributes...>(&drawcall, this);
		}

		template<typename _shape, typename ... _attributes>
		SceneDrawcall<_shape, _attributes...> GetDrawcall()
		{
			auto& drawcall = m_drawcalls[GetCombinedClassUUID<_shape, _attributes...>()];

			return SceneDrawcall<_shape, _attributes...>(&drawcall, this);
		}

	private:
		std::unordered_map<uint32_t, Drawcall> m_drawcalls;
		entt::registry m_registry;

		template<typename, typename...>
		friend class SceneDrawcall;
	};

};
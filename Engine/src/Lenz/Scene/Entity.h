#pragma once
#include "entt.hpp"
#include "Lenz/Event/EventAdaptor.h"

namespace lenz {

	class Scene;

	class Entity : public EventAdaptor
	{
	public:
		Entity() = default;
		Entity(const Entity&);
		Entity(Scene* scene, entt::entity handle);

		void Init(Scene* scene);

		template<typename T>
		void Init(Scene* scene)
		{
			if (m_Scene != nullptr)
			{
				LZ_CORE_ASSERT(false, "The entity already belong to a scene");
				return;
			}

			m_Scene = scene;
			Entity& e = scene->CreateEntity<T>();
			m_EntityHandle = e.m_EntityHandle;

			e.m_EntityHandle = (entt::entity)UINT_MAX;
			e.m_Scene = nullptr;
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			LZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		void AddVoidComponent(Args&&... args)
		{
			LZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			LZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return (uint32_t)m_EntityHandle != 0; }
		void operator = (Entity& entity) { m_EntityHandle = entity.m_EntityHandle; m_Scene = entity.m_Scene; }
		void operator = (const Entity& entity) { m_EntityHandle = entity.m_EntityHandle; m_Scene = entity.m_Scene; }

		inline entt::entity GetHandle() const { return m_EntityHandle; }

	private:
		entt::entity m_EntityHandle{ 0 };
		Scene* m_Scene = nullptr;
	};

}

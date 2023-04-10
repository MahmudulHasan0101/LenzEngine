#pragma once
#include "entt.hpp"
#include "Lenz/Event/EventAdaptor.h"

namespace lenz {

	class Entity : public EventAdaptor
	{
	public:
		Entity() = default;
		Entity(const Entity&);
		Entity(entt::registry* handle);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			LZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		void AddVoidComponent(Args&&... args)
		{
			LZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			LZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Registry->get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Registry->has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Registry->remove<T>(m_EntityHandle);
		}


		inline entt::entity GetHandle() const { return m_EntityHandle; }

	private:
		entt::entity m_EntityHandle{ 0 };
		entt::registry* m_Registry = nullptr;
	};

}

#pragma once
#include <entt/entt.hpp>
#include "Lenz/Event/EventAdaptor.h"
#include "Lenz/Utils/TimeStep.h"

namespace lenz {

	class Entity : public EventAdaptor
	{
	public:
		Entity() = default;
		Entity(const Entity&);
		Entity(const entt::entity& entity);
		Entity(const entt::entity& entity, entt::registry* reg);
		Entity(entt::registry* handle);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		void AddVoidComponent(Args&&... args)
		{
			m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
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
			m_Registry->remove<T>(m_EntityHandle);
		}

		template<typename T, typename... Args>
		T& GetOrEmplaceComponent(Args&&... args)
		{
			return m_Registry->get_or_emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		
		template<typename T, typename... Args>
		T& c(Args&&... args)
		{
			return m_Registry->get_or_emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}


		virtual void onUpdate(lenz::TimeStep& timestep) {};

		inline entt::entity GetHandle() const { return m_EntityHandle; }

	private:
		entt::entity m_EntityHandle{ 0 };
		entt::registry* m_Registry = nullptr;
	};






}

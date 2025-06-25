#include "Entity.h"
#include "Component.h"


namespace lenz {

	Entity::Entity(entt::registry* reg)
		: m_Registry(reg), m_EntityHandle(reg->create())
	{
	}

	Entity::Entity(const entt::entity& entity)
		: m_EntityHandle(entity) 
	{
	}

	Entity::Entity(const entt::entity& entity, entt::registry* reg)
		: m_EntityHandle(entity), m_Registry(reg)
	{
	}

	Entity::Entity(const Entity& e)
		: m_Registry(e.m_Registry), m_EntityHandle(e.m_EntityHandle)
	{
	}



}
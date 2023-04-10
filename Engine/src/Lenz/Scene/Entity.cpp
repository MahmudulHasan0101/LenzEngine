#include "Entity.h"


namespace lenz {

	Entity::Entity(entt::registry* reg)
		: m_Registry(reg), m_EntityHandle(reg->create())
	{
	}

	Entity::Entity(const Entity& e)
		: m_Registry(e.m_Registry), m_EntityHandle(e.m_EntityHandle)
	{
	}
}
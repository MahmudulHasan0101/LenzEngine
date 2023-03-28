#include "Entity.h"
#include "Scene.h"


namespace lenz {

	Entity::Entity(Scene* scene, entt::entity handle)
		: m_Scene(scene), m_EntityHandle(handle)
	{
	}

	Entity::Entity(const Entity& e)
		: m_Scene(e.m_Scene), m_EntityHandle(e.m_EntityHandle)
	{
	}

	void Entity::Init(Scene* scene)
	{
		m_Scene = (scene);
		Entity e = scene->CreateEntity();
		m_EntityHandle = e.m_EntityHandle;

		e.m_EntityHandle = (entt::entity)UINT_MAX;
		e.m_Scene = nullptr;
	}


}
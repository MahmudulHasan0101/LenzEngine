#include "Scene.h"
#include "DrawCall.h"

#include <glm/glm.hpp>

namespace lenz {
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity& Scene::CreateEntity()
	{
		Entity entity = { this, m_Registry.create() };
		return entity;
	}


}
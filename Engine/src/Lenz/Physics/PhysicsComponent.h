#pragma once
#include"Lenz/ECS/Component.h"
#include"glm/glm.hpp"

namespace lenz
{
	struct Rectangle
	{
		glm::vec3 position;
		float width, height;
	};


	struct PositionComponent
	{
		float x, y, z;

		ComponentID s_ComponentID;
	};

	struct ColorComponent
	{
		float r, g, b, a;

		ComponentID s_ComponentID;
	};

	struct HitboxComponent
	{
		Rectangle rect;

		ComponentID s_ComponentID;
	};

}

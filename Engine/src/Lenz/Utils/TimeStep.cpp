#include "TimeStep.h"
#include"Lenz/Core.h"

namespace lenz
{
	float TimeStep::m_Time = 0;
	float TimeStep::m_DeltaTime = 0;

	void TimeStep::Tick()
	{
		float time = glfwGetTime();
		m_DeltaTime = time - m_Time;
		m_Time = time;
	}
}
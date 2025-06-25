#pragma once
#include<Lenz/Core.h>


namespace lenz
{
	class TimeStep
	{
	public:
		TimeStep()
		{
		}
		inline float GetSeconds() const
		{
			return m_Time;
		}
		inline float GetMilliseconds() const
		{
			return m_Time * 1000.0f;
		}
		inline float GetDeltaSecond() const
		{
			return m_DeltaTime;
		}
		inline float GetDeltaMillisecond() const
		{
			return m_DeltaTime * 1000.0f;
		}
		operator float()
		{
			return m_DeltaTime;
		}

	private:
		static void Tick();
		friend class App;

		static float m_DeltaTime, m_Time;
	};
}

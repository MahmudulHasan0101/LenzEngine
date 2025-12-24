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


		static void SleepSeconds(double seconds)
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
		}

		static void SleepMilliseconds(uint64_t milliseconds)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
		}

	private:
		static void Tick();
		friend class App;

		static float m_DeltaTime, m_Time;
	};
}

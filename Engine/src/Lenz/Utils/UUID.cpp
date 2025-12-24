#include "UUID.h"
#include <random>
#include <unordered_map>
#include <chrono>

namespace lenz {

	//static std::random_device s_RandomDevice;
	//static std::mt19937_64 s_Engine(s_RandomDevice());
	//static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	//static std::hash<uint64_t> s_HashUint64_t;

	const static auto then = std::chrono::high_resolution_clock::now();


	UUID::UUID()
	{
		auto now = std::chrono::high_resolution_clock::now();
		m_UUID = std::chrono::duration_cast<std::chrono::nanoseconds>(now-then).count();
		//m_UUID = s_HashUint64_t(m_UUID);
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}

}
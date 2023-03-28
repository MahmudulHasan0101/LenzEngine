#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace lenz
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};

// Core logs macros:

	#define LZ_CORE_ERROR(arg)  Log::GetCoreLogger().get()->error(arg);
	#define LZ_CORE_WARN(arg)   Log::GetCoreLogger().get()->warn(arg);
	#define LZ_CORE_INFO(arg)   Log::GetCoreLogger().get()->info(arg);
	#define LZ_CORE_TRACE(arg)  Log::GetCoreLogger().get()->trace(arg);
	#define LZ_CORE_FATAL(arg)  Log::GetCoreLogger().get()->fatal(arg);

// Core logs macros:
	#define LZ_ERROR(arg)  lenz::Log::GetClientLogger().get()->error(arg);
	#define LZ_WARN(arg)   lenz::Log::GetClientLogger().get()->warn(arg);
	#define LZ_INFO(arg)   lenz::Log::GetClientLogger().get()->info(arg);
	#define LZ_TRACE(arg)  lenz::Log::GetClientLogger().get()->trace(arg);
	#define LZ_FATAL(arg)  lenz::Log::GetClientLogger().get()->fatal(arg);

}

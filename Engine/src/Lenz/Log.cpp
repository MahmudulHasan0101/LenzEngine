#include "Log.h"


std::shared_ptr<spdlog::logger> lenz::Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> lenz::Log::s_ClientLogger;


void lenz::Log::Init() 
{
	/* This pattern have, the time stamp, this name, and last the statement*/
	spdlog::set_pattern("%^[%T] %n: %v%$");

	s_CoreLogger = spdlog::stdout_color_mt("Engine");
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger = spdlog::stderr_color_mt("App");
	s_ClientLogger->set_level(spdlog::level::trace);
}


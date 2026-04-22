#include "ezpch.h"


namespace Ezzoo {

	std::shared_ptr<spdlog::logger> (Log::s_CoreLogger);
	std::shared_ptr<spdlog::logger> (Log::s_ClientLogger);


	void Log::Init()
	{
		spdlog::set_pattern("%n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("EZZOO");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stderr_color_mt("Application");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

	
}
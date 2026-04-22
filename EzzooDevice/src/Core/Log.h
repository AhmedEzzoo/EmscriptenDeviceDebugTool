#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

namespace Ezzoo {
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


}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<< (OStream& os, const glm::vec<L, T, Q>& vector)
{

	return  os << glm::to_string(vector);

}

#ifndef __EMSCRIPTEN__
	#define EZZOO_CORE_WARNING(...) ::Ezzoo::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define EZZOO_CORE_INFO(...)    ::Ezzoo::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define EZZOO_CORE_ERROR(...)   ::Ezzoo::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define EZZOO_CORE_TRACE(...)   ::Ezzoo::Log::GetCoreLogger()->trace(__VA_ARGS__)
	
	#define EZZOO_CLIENT_WARNING(...) ::Ezzoo::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define EZZOO_CLIENT_INFO(...)	  ::Ezzoo::Log::GetClientLogger()->info(__VA_ARGS__)
	#define EZZOO_CLIENT_ERROR(...)   ::Ezzoo::Log::GetClientLogger()->error(__VA_ARGS__)
	#define EZZOO_CLIENT_TRACE(...)   ::Ezzoo::Log::GetClientLogger()->trace(__VA_ARGS__)


#else 
	#define EZZOO_CORE_WARNING(...) 
	#define EZZOO_CORE_INFO(...)    
	#define EZZOO_CORE_ERROR(...)   
	#define EZZOO_CORE_TRACE(...)   

	#define EZZOO_CLIENT_WARNING(...) 
	#define EZZOO_CLIENT_INFO(...)	  
	#define EZZOO_CLIENT_ERROR(...)   
	#define EZZOO_CLIENT_TRACE(...)   
#endif
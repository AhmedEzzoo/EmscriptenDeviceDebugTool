#pragma once

#include "Log.h"
#include <memory>

//Bind Functions Lambda for Better performance 
#define EZZOO_BIND(fn)  [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...);}

#define BIT(x) (1 << x)


//Configuration 
#ifdef EZZOO_DEBUG
	#ifndef __EMSCRIPTEN__
		#define EZZOO_ASSERT_ENABLE
	#endif
#endif

#ifdef EZZOO_ASSERT_ENABLE
	#define EZZOO_CORE_ASSERT(x, ...)  { if(!(x)) { EZZOO_CORE_ERROR(__VA_ARGS__); __debugbreak(); } }
#else 
	#define EZZOO_CORE_ASSERT(x, y)
#endif

//Profile Enabling in Release configuration 
#ifdef EZZOO_RELEASE 
	#define EZZOO_PROFILE_ENABLE
#endif


namespace Ezzoo {

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ...ARGS>
	Ref<T> CreateRef(ARGS &&...args)
	{
		return std::make_shared<T>(std::forward<ARGS>(args)...);
	}


	template<typename T>
	using Scop = std::unique_ptr<T>;

	template <typename T, typename ...ARGS>
	Scop<T> CreateScope(ARGS&& ...args)
	{
		return std::make_unique<T>(std::forward<ARGS>(args)...);
	}
}
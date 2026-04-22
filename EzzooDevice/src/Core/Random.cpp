#include "ezpch.h"


#include "Random.h"

namespace Ezzoo {

	std::mt19937 Random::s_Engine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;



}
#pragma once

#include <random>
#include "glm/glm.hpp"
namespace Ezzoo {


	class Random
	{
	public :

		static void Init()
		{
			s_Engine.seed(std::random_device()());
		}

		static uint32_t UInt()
		{
			return s_Distribution(s_Engine);
		}

		static uint32_t UInt(uint32_t min, uint32_t max)
		{
			return min + (s_Distribution(s_Engine) % ((max - min + 1)));
		}

		static float Float()
		{
			return (float)(s_Distribution(s_Engine)) / (float)(std::numeric_limits<uint32_t>::max());
		}

		static glm::vec3 Vec3()
		{
			return glm::vec3(Float(), Float(), Float());
		}


		static glm::vec3 Vec3(float min, float max)
		{
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

	private :

		static std::mt19937 s_Engine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;



	};

}
#include "ezpch.h"

#include "UUID.h"
#include "Core/Random.h"

#include <unordered_map>


//static std::unordered_map<Ezzoo::UUID, std::string> s_IDSNames;

namespace Ezzoo {

	static std::random_device s_Engine;
	static std::mt19937 s_Random(s_Engine());
	static std::uniform_int_distribution<uint64_t> rNum;

	UUID::UUID() 
		: m_EntityID (rNum(s_Random))
	{
		//s_IDSNames[UUID()] = "Ahmed";
	}

	UUID::UUID(uint64_t ID)
		: m_EntityID(ID)
	{

	}

	UUID::~UUID()
	{

	}


	
}
#pragma once

#include <random>


//#include <xhash>


namespace Ezzoo {


	class UUID
	{
	public :

		UUID();
		UUID(uint64_t ID);
		UUID(const UUID& other) = default;
		
		~UUID();

		uint64_t GetEntityID() const { return m_EntityID; }
	


		operator uint64_t () const {

			return m_EntityID;
		}

	private :

		uint64_t m_EntityID;


	};

}
namespace std {

	template <>
	struct hash<Ezzoo::UUID>
	{
		std::size_t  operator()(const Ezzoo::UUID& id) const  
		{
			return hash<uint64_t>()((uint64_t)id);
		}
	};
}
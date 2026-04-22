
#include "ezpch.h"
#include "Entity.h"

#include "Component.h"

namespace Ezzoo {

	Entity::Entity(entt::entity entity, Scene* scene)
		: m_Entity(entity), m_Scene(scene)
	{}

	UUID& Entity::GetUUID()
	{
		return GetComponent<IDComponent>().ID;
	}
}
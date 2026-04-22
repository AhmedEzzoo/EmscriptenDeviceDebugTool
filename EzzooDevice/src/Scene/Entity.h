#pragma once

#include "Scene.h"

namespace Ezzoo {

	class Entity
	{

	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene);
		~Entity() = default;


		template <typename T, typename ...ARGS>
		T& AddComponent(ARGS &&...args)
		{
			T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<ARGS>(args)...);
			m_Scene->OnComponentAdded(*this, component);
			return component;
		}


		template <typename T, typename ...ARGS>
		T& AddOrReplaceComponent(ARGS &&...args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_Entity, std::forward<ARGS>(args)...);
			m_Scene->OnComponentAdded(*this, component);
			return component;
		}


		template <typename T>
		T& GetComponent()
		{ 
			EZZOO_CORE_ASSERT(HasComponent<T>(), "Requested Component is Not Available");
			return m_Scene->m_Registry.get<T>(m_Entity);


		}

		template <typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_Entity);
		}


		template <typename T>
		void RemoveComponent()
		{
			m_Scene->m_Registry.remove<T>(m_Entity);
		}
		
		 UUID& GetUUID();

		operator uint32_t()
		{
			return (uint32_t)m_Entity;
		}
		
		operator entt::entity()
		{
			return m_Entity;
		}

		bool operator== (const Entity& other)
		{
			return (m_Entity == other.m_Entity) && (m_Scene == other.m_Scene);
		}

		bool operator!= (const Entity& other)
		{
			return !(*this == other);
		}

		operator bool()
		{
			return m_Entity != entt::null;
		}



	private :

		entt::entity m_Entity{ entt::null };
		Scene* m_Scene = nullptr;

	};
}
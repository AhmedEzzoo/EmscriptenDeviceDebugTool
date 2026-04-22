#pragma once

#include "entt.hpp"
#include "Core/TimeStep.h"
#include "Renderer/EditorCamera.h"
#include "Core/UUID.h"


namespace Ezzoo {


	class Entity;

	class Scene
	{
	public :

		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithID(const UUID& id, const std::string& name);

		static Ref<Scene> Copy(Ref<Scene> other);
		Entity DuplicateEntity(Entity src);

		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateRunTime(TimeStep ts);
		void OnUpdateSimulate(TimeStep ts, EditorCamera& camera);

		Entity GetEntityByID(UUID id);
		Entity GetEntityByname(std::string_view name);

		void DestroyEntity(Entity entity);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetCameraPrimaryEntity();

		void OnRunTimeStart();
		void OnRunTimeStop();
		void OnSimulationStart();
		void OnSimulationStop();

		void SetPausing(bool pause) { m_IsPausing = pause; }
		bool IsPaused() { return m_IsPausing ; }
		void SetStepFrames(int8_t frames) { m_StepFrames = frames; }

		

		template <typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	public :
		bool m_IsRunning = false;

	private:
		template <typename T>
		void OnComponentAdded(Entity entity, T& component);
	private :

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0 , m_ViewportHeight = 0;

		std::unordered_map<UUID, entt::entity> m_EnttMap;
		bool m_IsPausing = false;
		int8_t m_StepFrames = 0;

		friend class Entity;
		friend class SceneSerialize;
		friend class SceneHierarchyPanel;




	};
}

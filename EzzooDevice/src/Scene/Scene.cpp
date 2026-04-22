#include "ezpch.h"
#include "Component.h"
#include "Entity.h"
#include "Renderer/Renderer2D.h"



namespace Ezzoo {


	Scene::Scene()
	{

		/*entt::entity ent = m_Registry.create();

		TransformComponent& transfom = m_Registry.emplace<TransformComponent>(ent);

		m_Registry.get<TransformComponent>(ent);

		auto view = m_Registry.view<TransformComponent>();

		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);

		}*/


	}

	Scene::~Scene()
	{
		
	}

	template <typename Component>
	static void CopyComponent(entt::registry& dstReg, entt::registry& srcReg, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = srcReg.view<Component>();
		for (auto entt : view)
		{
			//EZZOO_CORE_ASSERT((enttMap.find(uuid) != enttMap.end()), "ID Does not exist");
			entt::entity e = enttMap.at(srcReg.get<IDComponent>(entt).ID);
			auto& component = srcReg.get<Component>(entt);

			dstReg.emplace_or_replace<Component>(e, component);
		}

	
	}

	
	template <typename... Component>
	static void CopyComponentIfExist(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}


	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcRegister = other->m_Registry;
		auto& dstRegister = newScene->m_Registry;

		auto srcView = srcRegister.view<IDComponent>();

		for (auto e : srcView)
		{
			UUID id = srcRegister.get<IDComponent>(e).ID;
			const auto& name = srcRegister.get<TagComponent>(e).TagName;

			Entity entt = newScene->CreateEntityWithID(id, name);
			enttMap[id] = (entt::entity)entt;
		}



		CopyComponent<TransformComponent>(dstRegister, srcRegister, enttMap);
		CopyComponent<SpriteRendererComponent>(dstRegister, srcRegister, enttMap);
		CopyComponent<CircleComponent>(dstRegister, srcRegister, enttMap);
		CopyComponent<CameraComponent>(dstRegister, srcRegister, enttMap);

		return newScene;
	
	}

	Entity Scene::DuplicateEntity(Entity src)
	{
		Entity newEntity = CreateEntity(src.GetComponent<TagComponent>().TagName);

		CopyComponentIfExist<TransformComponent, SpriteRendererComponent, CircleComponent, CameraComponent>(newEntity, src);

		return newEntity;

	}


	Entity Scene::CreateEntity(const std::string& name)
	{
		
		return CreateEntityWithID(UUID(), name);
	}

	Entity Scene::CreateEntityWithID(const UUID& id, const std::string& name)
	{
		Entity entity{ m_Registry.create(), this };
		entity.AddComponent<IDComponent>(id);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.TagName = name.empty() ? "Entity" : name;

		m_EnttMap[id] = entity;


		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
		m_EnttMap.erase(entity.GetComponent<IDComponent>().ID);
	}
	void Scene::OnRunTimeStart()
	{

		m_IsRunning = true;
	}

	void Scene::OnRunTimeStop()
	{
		m_IsRunning = false;
	
	}

	void Scene::OnSimulationStart()
	{
		//OnPhysics2DRender();

	}


	void Scene::OnSimulationStop()
	{
		//delete m_Physicsworld;

	}
	

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{

		Renderer2D::BeginScene(camera);

		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{

				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				//EZZOO_CLIENT_WARNING("{0}", sprite.Color.x);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite, (int)entity);
			}
		}

		{

			auto view = m_Registry.view<TransformComponent, CircleComponent>();
			for (auto entity : view)
			{

				auto [transform, circle] = view.get<TransformComponent, CircleComponent>(entity);
				//EZZOO_CLIENT_WARNING("{0}", sprite.Color.x);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade , (int)entity);
			}

		}


		//Renderer2D::DrawLine(glm::vec3(1.0f), glm::vec3(2.0f), glm::vec4(0.8f, 0.2f, 0.3f, 1.0f ));
		//Renderer2D::DrawPoints(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		
		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRunTime(TimeStep ts)
	{

		if (!m_IsPausing || m_StepFrames > 0)
		{

		}
		//Renderer 2D
		Ezzoo::Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{

			auto view = m_Registry.view<TransformComponent, CameraComponent>();

			for (auto entity : view)
			{
				auto [trans, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.PrimaryCamera)
				{
					mainCamera = &camera.Camera;
					cameraTransform = trans.GetTransform();
					break;
				}
			}

		}

		

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [trans, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawQuad(trans.GetTransform(), color, (int)entity);
				}

			}

			{
				auto view = m_Registry.view<TransformComponent, CircleComponent>();
				for (auto entity : view)
				{
					auto [trans, circle] = view.get<TransformComponent, CircleComponent>(entity);

					Renderer2D::DrawCircle(trans.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}

			}
			
			Renderer2D::EndScene();
		}
	
		
	}





	void Scene::OnUpdateSimulate(TimeStep ts, EditorCamera& camera)
	{



		//Renderer 2D
		Renderer2D::BeginScene(camera);

		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{

				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				//EZZOO_CLIENT_WARNING("{0}", sprite.Color.x);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite, (int)entity);
			}
		}

		{

			auto view = m_Registry.view<TransformComponent, CircleComponent>();
			for (auto entity : view)
			{

				auto [transform, circle] = view.get<TransformComponent, CircleComponent>(entity);
				//EZZOO_CLIENT_WARNING("{0}", sprite.Color.x);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}

		}

		Renderer2D::EndScene();
	}


	Entity Scene::GetEntityByID(UUID id)
	{
		if (m_EnttMap.find(id) != m_EnttMap.end())
		{
			Entity ent = { m_EnttMap.at(id), this };
			return ent;
		}
		return {};
	}

	Entity Scene::GetEntityByname(std::string_view name)
	{
		const auto& view = m_Registry.view<TagComponent>();
		for (const auto& entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);

			if (tc.TagName == name)
				return Entity{ entity, this };
		}

		return {};

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{		
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();

		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.FixedAspectRation)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetCameraPrimaryEntity()
	{
		auto view = m_Registry.view<CameraComponent>();

		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);

			if (camera.PrimaryCamera)
			{
				return Entity { entity, this };

			}
		}

		return {};

	}


	template <typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(true);
	}

	template <>
	void Scene::OnComponentAdded(Entity entity, TransformComponent& component)
	{
	}
	template <>
	void Scene::OnComponentAdded(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template <>
	void Scene::OnComponentAdded(Entity entity, TagComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded(Entity entity, SpriteRendererComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded(Entity entity, CircleComponent& component)
	{
	}
	
	template <>
	void Scene::OnComponentAdded(Entity entity, NativeScriptComponent& component)
	{
	}

	template <>
	void Scene::OnComponentAdded(Entity entity, IDComponent& component)
	{
	}


}
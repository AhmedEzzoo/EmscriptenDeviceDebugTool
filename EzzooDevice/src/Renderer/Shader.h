#pragma once
#include <string>

#include "Core/Core.h"

#include "glm/glm.hpp"

namespace Ezzoo {

	class Shader
	{
	public :

		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const  = 0;
		virtual const std::string& GetName() const = 0;

		virtual void SetFloat(const std::string& name, float value) =  0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& values) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& values) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& values) = 0;

		virtual void SetInt(const std::string& name, int vlaue) = 0;
		virtual void SetIntArr(const std::string& name , int* arrValue, uint32_t maxSize) = 0;

		virtual void SetMat3(const std::string& name, const glm::mat3& values) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& values) = 0;


		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filepath);
	};

	class ShaderLibrary
	{
	public :
		
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader>& Get(const std::string& name);

	private :
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);

	private :
		std::unordered_map<std::string,  Ref<Shader>> m_Shaders;
	};
}

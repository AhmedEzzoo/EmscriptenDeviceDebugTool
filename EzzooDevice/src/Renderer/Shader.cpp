#include "ezpch.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"


namespace Ezzoo {

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RendererAPI::GetAPI()) 
		{
		case RendererAPI::API::None: EZZOO_CORE_ASSERT(false, "None is not Renderer Type"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		EZZOO_CORE_ASSERT(false, "This not valid Renderer!!!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: EZZOO_CORE_ASSERT(false, "None is not Renderer Type"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
		}

		EZZOO_CORE_ASSERT(false, "This not valid Renderer!!!");
		return nullptr;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto name = shader->GetName();
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		EZZOO_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Name already Exist");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader->GetName(), shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		EZZOO_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader is not in Map");
		auto shader = Shader::Create(filepath);
		Add(name, shader);

		return shader;
	}

	Ref<Shader>& ShaderLibrary::Get (const std::string& name)
	{
		EZZOO_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader is not in Map");
		return m_Shaders[name];
	}

}
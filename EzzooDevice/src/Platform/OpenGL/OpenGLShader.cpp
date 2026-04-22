#include "ezpch.h"

#include "OpenGLShader.h"

#include "Core/Core.h"
#include "Core/Timer.h"


#include <glm/gtc/type_ptr.hpp>

#ifndef __EMSCRIPTEN__
	#include <glad/glad.h>
#else 
	#include "GLES3/gl3.h"
	#include "emscripten.h"
#endif

//#include <shaderc/shaderc.hpp>
//#include <spirv_cross/spirv_cross.hpp>
//#include <spirv_cross/spirv_glsl.hpp>

#include <filesystem>

namespace Ezzoo {


	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string shaderStr =  ReadFile(filepath);
		auto shaders = PreProcess(shaderStr);

		Compile(shaders);

		size_t lastSlash = filepath.find_last_of("/\\");
		EZZOO_CORE_ASSERT(lastSlash != std::string::npos, "No Slashes in File path, Wrong file path format!!!");

		size_t firstLetterIndex = lastSlash + 1;
		size_t dot = filepath.find_last_of(".");
		EZZOO_CORE_ASSERT(dot != std::string::npos, "No Dot in File path, Wrong file path format!!!");

		std::string name = filepath.substr(firstLetterIndex, (dot - firstLetterIndex));

		m_Name = name;

	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
	{
		std::string vertexStr = ReadFile(vertexSrc);
		std::string fragStr = ReadFile(fragmentSrc);
		//EZZOO_CLIENT_INFO("Vertex: {}", vertexStr.c_str());
		//EZZOO_CLIENT_INFO("Frag: {}", fragStr.c_str());
		std::unordered_map<GLenum, std::string> shaders;
		shaders[GL_VERTEX_SHADER] = vertexStr;
		shaders[GL_FRAGMENT_SHADER] = fragStr;

		Compile(shaders);
			
	}
	OpenGLShader::~OpenGLShader()
	{
			glDeleteProgram(m_RendererID);
	}

	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment")
			return GL_FRAGMENT_SHADER;

		EZZOO_CORE_ASSERT(false, "Not Valid shader Type!!!");

		return 0;
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string shaderStr;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);

		if (in)
		{
			in.seekg(0, std::ios::end);
			shaderStr.resize(in.tellg());

			in.seekg(0, std::ios::beg);
			in.read(&shaderStr[0], shaderStr.size());

			in.close();
		}
		else
		{
			EZZOO_CORE_ASSERT(false, "File is not available in this Path "+ filepath +"!!!");
		}

		return shaderStr;

	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& result)
	{
			std::unordered_map<GLenum, std::string> resultShaderMap;

			const char* typeToken = "#type";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = result.find(typeToken, 0);

			//#type vertex

			while (pos != std::string::npos)
			{
				size_t eol = result.find_first_of("\r\n", pos);
				EZZOO_CORE_ASSERT(eol != std::string::npos, "Syntax Error");

				size_t begin = pos + typeTokenLength + 1;
				std::string type = result.substr(begin, (eol - begin));
				EZZOO_CORE_ASSERT(type == "vertex" || type == "fragment", "Invalid Shader Type!!!");

				size_t nextLinePos = result.find_first_not_of("\r\n", eol);
				pos = result.find(typeToken, nextLinePos);

				resultShaderMap[ShaderTypeFromString(type)] = result.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? result.size() : nextLinePos));
			}
			
			return resultShaderMap;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaders)
	{
			GLint program = glCreateProgram();
			EZZOO_CORE_ASSERT(shaders.size() <= 2, "We only support 2 shaders for now");
			std::array<GLenum, 2> shaderIDs;
			// GLenum *shaderIDs = (GLenum *)alloca(shaders.size() * sizeof(GLenum)); // alternative to allocate on stack
			int shaderIDIndex = 0;
			for (auto& kv : shaders)
			{

				GLenum type = kv.first;
				const std::string& source = kv.second;

			
				GLuint shader = glCreateShader(type);
				
				const GLchar* sourceCstr = source.c_str();
				glShaderSource(shader, 1, &sourceCstr, 0);
				
				glCompileShader(shader);

				GLint isCompiled = 0;
				
				glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
				if (isCompiled == GL_FALSE)
				{
					GLint maxLength = 0;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				
					std::vector<GLchar> infoLog(maxLength);
					glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

					
					glDeleteShader(shader);

					EZZOO_CORE_ERROR("{0}", infoLog.data());
					EZZOO_CORE_ASSERT(false, "Shader compilation faild");
					break;
				}

				glAttachShader(program, shader);
				shaderIDs[shaderIDIndex++] = shader;
			}

			// Link our program
			glLinkProgram(program);

			// Note the different functions here: glGetProgram* instead of glGetShader*.
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);

				for (auto id : shaderIDs)
					// Don't leak shaders either.
					glDeleteShader(id);

				// Use the infoLog as you see fit.

				// In this simple program, we'll just leave
				EZZOO_CORE_ERROR("{0}", infoLog.data());
				EZZOO_CORE_ASSERT(false, "Shader Link compilation faild");
				return;
			}

			// Always detach shaders after a successful link.
			for (auto id : shaderIDs)
				glDetachShader(program, id);

			m_RendererID = program;
	}
	
	#if 0
	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			EZZOO_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			EZZOO_CORE_ASSERT(false, "Unknown shader type!");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			EZZOO_CORE_ASSERT(false, "Unknown shader type!");
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "D:/C++VisualStudio/dev/EzzooNut/EzzooDevice/ShaderAssets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			EZZOO_CORE_ASSERT(false, "Unknown shader type!");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			EZZOO_CORE_ASSERT(false, "Unknown shader type!");
			return "";
		}


	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_FilePath(filepath)
	{
		//EZZOO_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			EZZOO_CORE_WARNING("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		//EZZOO_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}

	OpenGLShader::~OpenGLShader()
	{
		//EZZOO_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		//EZZOO_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				EZZOO_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			EZZOO_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		//EZZOO_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			EZZOO_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			EZZOO_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			EZZOO_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					EZZOO_CORE_ERROR(module.GetErrorMessage());
					EZZOO_CORE_ASSERT(false, "Not Found");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					EZZOO_CORE_ERROR(module.GetErrorMessage());
					//EZZOO_CORE_ASSERT(false);
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			EZZOO_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		EZZOO_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		EZZOO_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		EZZOO_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		EZZOO_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			EZZOO_CORE_TRACE("  {0}", resource.name);
			EZZOO_CORE_TRACE("    Size = {0}", bufferSize);
			EZZOO_CORE_TRACE("    Binding = {0}", binding);
			EZZOO_CORE_TRACE("    Members = {0}", memberCount);
		}
	}
	#endif

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	/*int OpenGLShader::GetLocation(const std::string& name)
	{
		int locationVal;

		if (m_CashedValues.find(name) != m_CashedValues.end())
				locationVal = m_CashedValues[name];
		else
		{
				locationVal = glGetUniformLocation(m_RendererID, name.c_str());
				m_CashedValues[name] = locationVal;
		}

		return locationVal;
	}*/

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& values)
	{
		UploadUniformFloat2(name, values);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& values)
	{
		UploadUniformFloat3(name, values);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& values)
	{
		UploadUniformFloat4(name, values);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArr(const std::string& name, int* arrValue, uint32_t maxSize)
	{
		UploadUniformIntArr(name, arrValue, maxSize);
	}

	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& values)
	{
		UploadUniformMat3(name, values);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& values)
	{
		UploadUniformMat4(name, values);
	}

	void OpenGLShader::UploadUniformIntArr(const std::string& name, int* arrValue, uint32_t maxSize)
	{
		GLint locationVal = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniform1iv(locationVal, maxSize, arrValue);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& values)
	{
		GLint locationVal = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniformMatrix3fv(locationVal, 1, GL_FALSE, glm::value_ptr(values));

	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& values)
	{
		GLint locationVar = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniformMatrix4fv(locationVar, 1, GL_FALSE, glm::value_ptr(values));

	}


	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint locationVar = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniform1f(locationVar, value);

	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		GLint locationVar = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniform2f(locationVar, values.x, values.y);

	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		GLint locationVar = glGetUniformLocation(m_RendererID, name.c_str()); //GetLocation(name);
		glUniform3f(locationVar, values.x, values.y, values.z);

	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		GLint locationVar = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniform4f(locationVar, values.x, values.y, values.z, values.w);

	}
	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint locationVar = glGetUniformLocation(m_RendererID, name.c_str());//GetLocation(name);
		glUniform1i(locationVar, value);
	}
}

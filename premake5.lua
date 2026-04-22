workspace "DeviceDebugTool"
	architecture "x86"
	configurations {"Debug", "Release", "Dist"}
	startproject "EzzooDevice"

--outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--include "EzzooDevice/vendor/GLFW"

project "EzzooDevice"
	location "EzzooDevice"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	
	--targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	--objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	targetdir "bin"--"bin"
	objdir "obj/Debug"--"obj"

	--pchheader "ezpch.h"
	--pchsource "EzzooDevice/src/ezpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/entt/include/entt.hpp",
		"%{prj.name}/vendor/imgui/imconfig.h",
		"%{prj.name}/vendor/imgui/imgui.h",
		"%{prj.name}/vendor/imgui/imgui.cpp",
		"%{prj.name}/vendor/imgui/imgui_draw.cpp",
		"%{prj.name}/vendor/imgui/imgui_internal.h",
		"%{prj.name}/vendor/imgui/imgui_widgets.cpp",
		"%{prj.name}/vendor/imgui/imstb_rectpack.h",
		"%{prj.name}/vendor/imgui/imstb_textedit.h",
		"%{prj.name}/vendor/imgui/imstb_truetype.h",
		"%{prj.name}/vendor/imgui/imgui_demo.cpp",
       		 "%{prj.name}/vendor/imgui/imgui_tables.cpp",

        	"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.cpp",
        	"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.h",
        	"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.cpp",
        	"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.h",
		--"%{prj.name}/vendor/GLad/include/glad/glad.h",
		--"%{prj.name}/vendor/GLad/include/KHR/khrplatform.h",
		--"%{prj.name}/vendor/GLad/src/glad.c"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		--"%{prj.name}/vendor/GLFW/include",
		--"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/stb_image",
		"%{prj.name}/vendor/entt/include",
		--"%{prj.name}/vendor/Glad/include",

		--"../vendor/GLFW/include",
        	"%{prj.name}/vendor/backends",
        	"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/emsdk/upstream/emscripten/cache/sysroot/include",
		"%{prj.name}/vendor/eigen",
		"%{prj.name}/vendor/json/include"


	}
	
	defines
	{
		"EZZOO_WINDOWS_64"
	}

	filter "system:linux"
		
		pchheader "ezpch.h"
		pchsource "EzzooDevice/src/ezpch.cpp"


		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"IMGUI_DEFINE_MATH_OPERATORS",
			"IMGUI_IMPL_OPENGL_ES3",
			"__EMSCRIPTEN__",
			"BROWSER_BUILD"
			
		}
		links
		{
			--"GLFW",
			--"opengl32.lib",
			--"opengl32"
		}
		targetextension ".html"
		buildoptions
		{
			"-s USE_GLFW=3",
			"-s USE_WEBGL2=1",
			"-s FULL_ES3=1",
			"-s GL_DEBUG=1",
			"-s NO_DISABLE_EXCEPTION_CATCHING",
			"--preload-file Resources/Shaders",
		}
		linkoptions
		{
			"-s USE_GLFW=3",
			"-s USE_WEBGL2=1", 
      		"-s MAX_WEBGL_VERSION=2",
			"-s MIN_WEBGL_VERSION=2",
      		"-s FULL_ES3=1", 
			"-s ALLOW_MEMORY_GROWTH=0",
			"-s NO_EXIT_RUNTIME=1",
			"-s ASSERTIONS=1",
			"-s GL_DEBUG=1",
			"--preload-file Resources/Shaders",
			"-lwebsocket.js"
		}

			--removefiles 
			--{
        	--	"%{prj.name}/vendor/GLFW/include/GLFW/**.h", -- remove GLFW source files
			--	"%{prj.name}/vendor/GLFW/src/**.c",
			--	"%{prj.name}/vendor/GLFW/src/**.h",
			--	"%{prj.name}/vendor/GLFW/src/**.m",
    		--}

	filter "configurations:Debug"
		defines "EZZOO_DEBUG"
		runtime "Debug"
		symbols "on"
		prebuildcommands
		{
			"rm -rf ../bin"
		}

	filter "configurations:Release"
		defines "EZZOO_RELEASE"
		runtime "Release"
		optimize "on"

	filter "files:EzzooDevice/vendor/imgui/**.cpp"
	flags {"NoPCH"}
	--filter "files:EzzooDevice/vendor/Glad/src/glad.c"
	--flags {"NoPCH"}

	filter "configurations:Dist"
		defines "EZZOO_DIST"
		runtime "Release"
		optimize "on"
	

workspace "TextureGraph"
	architecture "x64"
	flags {"MultiProcessorCompile"}

	configurations 
	{
		"Debug", 
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}"

project "TextureGraph"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/" .. outputdir)
	debugdir "%{cfg.targetdir}"

	buildoptions { "/bigobj" }

	postbuildcommands {
	  "{MKDIR} %{cfg.targetdir}/resources",
	  "{MKDIR} %{cfg.targetdir}/library",
	  "{MKDIR} %{cfg.targetdir}/library/Nodes",
	  "{COPY} resources %{cfg.targetdir}/resources",
	  "{COPY} src/Nodes %{cfg.targetdir}/library/Nodes"
	}

	files 
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/imgui/**.cpp",
		"vendor/imgui/**.h",
		"src/**.tgnode"
	}

	includedirs
	{
		"vendor/sfml/include/",
		"vendor/spdlog/include",
		"vendor/imgui",
		"vendor/nfd/src/include",
		"vendor/lua/include"
	}

	libdirs { "vendor/sfml/extlibs/libs-msvc-universal/x64", "vendor/sfml/lib", "vendor/nfd/build/src/Release", "vendor/lua" }
	links { "opengl32.lib", "freetype.lib", "winmm.lib", "gdi32.lib", "openal32.lib", "flac.lib", "vorbisenc.lib", "vorbisfile.lib", "vorbis.lib", "ogg.lib", "ws2_32.lib", "legacy_stdio_definitions.lib", "nfd.lib", "lua54.lib" }


	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		defines "SFML_STATIC"

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
		staticruntime "on"
		runtime "Debug"

		links { "sfml-graphics-s-d.lib", "sfml-system-s-d.lib", "sfml-window-s-d.lib" }

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"
		staticruntime "on"
		runtime "Release"

		links { "sfml-graphics-s.lib", "sfml-system-s.lib", "sfml-window-s.lib" }

	filter "configurations:Dist"
		defines "DIST"
		optimize "On"
		staticruntime "on"
		runtime "Release"

		links { "sfml-graphics-s.lib", "sfml-system-s.lib", "sfml-window-s.lib" }
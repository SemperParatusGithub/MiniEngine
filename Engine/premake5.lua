project "Engine"
	location "../Scripts/Build"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "on"

	targetdir   ("../Binaries/" .. outputdir)
	objdir      ("../Binaries/Intermediate/" .. outputdir)

	pchheader "Precompiled.h"
	pchsource "Source/Precompiled.cpp"

	files
	{
		-- Source Files
		"Source/**.h",
		"Source/**.cpp",

		-- Cereal
		"External/cereal/include/cereal/**.h",
		"External/cereal/include/cereal/**.hpp",
		"External/cereal/include/cereal/**.cpp",

		-- entt
		"External/entt/include/entt/entt.hpp",

		-- glm
		"External/glm/glm/**.hpp",
		"External/glm/glm/**.inl",

		--box2d
		"External/box2d/include/box2d/**.h",
		"External/box2d/src/**.cpp",

		-- ImGuizmo
		"External/ImGuizmo/ImGuizmo.h",
		"External/ImGuizmo/ImGuizmo.cpp",
		
		-- STB
		"External/stb_image/**.h",
		"External/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"Source",

		"../Engine/External/GLFW/include",
		"../Engine/External/Glad/include",
		"../Engine/External/imgui",		
        "../Engine/External/glm",
        "../Engine/External/stb_image",
		"../Engine/External/entt/include",
		"../Engine/External/ImGuizmo",
		"../Engine/External/cereal/include",
		"../Engine/External/assimp/include",
		"../Engine/External/box2d/include"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"Box2D",
		"opengl32.lib"
	}

	filter "files:External/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ME_DEBUG"
		runtime "Debug"
		symbols "on"
		postbuildcommands
		{
			'{COPY} "../../Engine/External/assimp/bin/Debug/assimp-vc142-mtd.dll" "%{cfg.targetdir}"',
			'{COPY} "../../Engine/External/assimp/bin/Debug/assimp-vc142-mtd.lib" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "ME_RELEASE"
		runtime "Release"
		optimize "on"
		postbuildcommands
		{
			'{COPY} "../../Engine/External/assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"',
			'{COPY} "../../Engine/External/assimp/bin/Release/assimp-vc142-mt.lib" "%{cfg.targetdir}"'
		}

	filter "configurations:Production"
		defines "ME_PRODUCTION"
		runtime "Release"
		optimize "on"
		postbuildcommands
		{
			'{COPY} "../../Engine/External/assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"',
			'{COPY} "../../Engine/External/assimp/bin/Release/assimp-vc142-mt.lib" "%{cfg.targetdir}"'
		}
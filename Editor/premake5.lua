project "Editor"
	location "../Scripts/Build"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir   ("../Binaries/" .. outputdir)
	objdir      ("../Binaries/Intermediate/" .. outputdir)

	files
	{
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs
	{
		"Source",
		"../Engine/Source",
		"../Engine/External",
		"../Engine/External/glm",
		"../Engine/External/Glad/include",
		"../Engine/External/GLFW/include",
		"../Engine/External/entt/include",
		"../Engine/External/cereal/include",
		"../Engine/External/assimp/include"
	}

	links
	{ 
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ME_DEBUG"
		runtime "Debug"
		symbols "on"
		links { "../Engine/External/assimp/bin/Debug/assimp-vc141-mtd.lib" }

	filter "configurations:Release"
		defines "ME_RELEASE"
		runtime "Release"
		optimize "on"
		links { "../Engine/External/assimp/bin/Release/assimp-vc141-mt.lib" }

	filter "configurations:Production"
		defines "ME_PRODUCTION"
		runtime "Release"
		optimize "on"
		links { "../Engine/External/assimp/bin/Release/assimp-vc141-mt.lib" }

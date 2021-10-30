project "Box2D"
	location "../../../Scripts/Build"
	kind "StaticLib"
	language "C++"
	warnings "off"

	targetdir ("../../../Binaries/" .. outputdir)
    objdir ("../../../Binaries/Intermediate/" .. outputdir)

	files
	{
		"include/**.h",
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"include",
		"src"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Production"
		runtime "Release"
		optimize "on"
workspace "MiniEngine"
	architecture "x86_64"
	startproject "Editor"

	configurations 	{ "Debug", "Release", "Production" }
	flags 			{ "MultiProcessorCompile" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "../Engine/External/GLFW"
	include "../Engine/External/Glad"
	include "../Engine/External/imgui"
	include "../Engine/External/Box2D"
group ""

group "Engine"
	include "../Engine"
group ""

group "Editor"
	include "../Editor"
group ""

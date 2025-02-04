-- premake5.lua
workspace "TerrainProject"
   location ("../../")
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "TerrainProject"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "../../Project/Build-Project.lua"


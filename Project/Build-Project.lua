project "TerrainProject"
   location( _SCRIPT_DIR )
   kind "ConsoleApp"
   language "C"
   cdialect  "C17"
   targetdir "Build/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.c" }

   includedirs
   {
      "src",
   }

   targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"
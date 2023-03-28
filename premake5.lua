workspace "Rush-hour"
  configurations { "Debug", "Release" }
  architecture "x86_64"

  project "Rush-hour"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "Code/**.cpp", "Code/**.h" }
    includedirs { "Code" }
    
    includedirs {"include"}
    libdirs { "lib/x64" }
    links { "SDL2.lib", "SDL2main.lib", "SDL2_image.lib" }
    links { "SDL2", "SDL2main", "SDL2_image"}

    filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

    configuration { "windows", "vs*" }
      location "build"
      debugdir "."
        
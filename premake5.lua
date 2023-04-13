workspace "Rush-hour"
  configurations { "Debug", "Release" }
  architecture "x86_64"
  location "build"
  
  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

  configuration { "windows", "vs*" }
    location "build"
    debugdir "."

  otherfiles = { "Code/GameSituation.cpp", "Code/GameSituation.h", "Code/Generator.h", "Code/Graph.h" }

  project "Rush-hour-test"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    files { otherfiles, "Code/RushHourTest.cpp" }

  project "Rush-hour-sdl"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    files { otherfiles, "Code/RushHourApp.cpp" }
    includedirs { "include" }
    libdirs { "lib/x64" }
    links { "SDL2.lib", "SDL2main.lib", "SDL2_image.lib" }
    links { "SDL2", "SDL2main", "SDL2_image"}

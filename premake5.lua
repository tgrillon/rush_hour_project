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

  project "Rush-hour-test"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    files { "Code/game_situation.cpp", "Code/game_situation.h", "Code/generator.h", "Code/main.cpp" }

  project "Rush-hour-sdl"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    files { "Code/game_situation.cpp", "Code/game_situation.h", "Code/generator.h", "Code/main_sdl.cpp" }
    includedirs { "include" }
    libdirs { "lib/x64" }
    links { "SDL2.lib", "SDL2main.lib", "SDL2_image.lib" }
    links { "SDL2", "SDL2main", "SDL2_image"}

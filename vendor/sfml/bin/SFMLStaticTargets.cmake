# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.6)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.21)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

if(CMAKE_VERSION VERSION_LESS 3.0.0)
  message(FATAL_ERROR "This file relies on consumers using CMake 3.0.0 or greater.")
endif()

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget SFML::System SFML::Main SFML::Window OpenGL SFML::Graphics Freetype)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target SFML::System
add_library(SFML::System STATIC IMPORTED)

set_target_properties(SFML::System PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "SFML_STATIC"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_INCLUDE_DIRECTORIES "D:/Git Projects/biosim/vendor/SFML/include"
  INTERFACE_LINK_LIBRARIES "\$<LINK_ONLY:winmm>"
)

# Create imported target SFML::Main
add_library(SFML::Main STATIC IMPORTED)

set_target_properties(SFML::Main PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "SFML_STATIC"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_INCLUDE_DIRECTORIES "D:/Git Projects/biosim/vendor/SFML/include"
)

# Create imported target SFML::Window
add_library(SFML::Window STATIC IMPORTED)

set_target_properties(SFML::Window PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "SFML_STATIC"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_INCLUDE_DIRECTORIES "D:/Git Projects/biosim/vendor/SFML/include"
  INTERFACE_LINK_LIBRARIES "SFML::System;\$<LINK_ONLY:OpenGL>;\$<LINK_ONLY:winmm>;\$<LINK_ONLY:gdi32>"
)

# Create imported target OpenGL
add_library(OpenGL INTERFACE IMPORTED)

set_target_properties(OpenGL PROPERTIES
  INTERFACE_LINK_LIBRARIES "opengl32"
)

# Create imported target SFML::Graphics
add_library(SFML::Graphics STATIC IMPORTED)

set_target_properties(SFML::Graphics PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "SFML_STATIC"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_INCLUDE_DIRECTORIES "D:/Git Projects/biosim/vendor/SFML/include"
  INTERFACE_LINK_LIBRARIES "SFML::Window;\$<LINK_ONLY:legacy_stdio_definitions.lib>;\$<LINK_ONLY:Freetype>"
)

# Create imported target Freetype
add_library(Freetype INTERFACE IMPORTED)

set_target_properties(Freetype PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "D:/Git Projects/biosim/vendor/SFML/extlibs/headers/freetype2"
  INTERFACE_LINK_LIBRARIES "D:/Git Projects/biosim/vendor/SFML/extlibs/libs-msvc-universal/x64/freetype.lib"
)

# Import target "SFML::System" for configuration "Debug"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "D:/Git Projects/biosim/vendor/SFML/bin/lib/Debug/sfml-system-s-d.lib"
  )

# Import target "SFML::Main" for configuration "Debug"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "D:/Git Projects/biosim/vendor/SFML/bin/lib/Debug/sfml-main-d.lib"
  )

# Import target "SFML::Window" for configuration "Debug"
set_property(TARGET SFML::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::Window PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "D:/Git Projects/biosim/vendor/SFML/bin/lib/Debug/sfml-window-s-d.lib"
  )

# Import target "SFML::Graphics" for configuration "Debug"
set_property(TARGET SFML::Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::Graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "D:/Git Projects/biosim/vendor/SFML/bin/lib/Debug/sfml-graphics-s-d.lib"
  )

# Import target "SFML::System" for configuration "Release"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "D:/Git Projects/biosim/vendor/SFML/bin/lib/Release/sfml-system-s.lib"
  )

# Import target "SFML::Main" for configuration "Release"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "D:/Git Projects/biosim/vendor/SFML/bin/lib/Release/sfml-main.lib"
  )

# Import target "SFML::Window" for configuration "Release"
set_property(TARGET SFML::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::Window PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "D:/Git Projects/biosim/vendor/SFML/bin/lib/Release/sfml-window-s.lib"
  )

# Import target "SFML::Graphics" for configuration "Release"
set_property(TARGET SFML::Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SFML::Graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "D:/Git Projects/biosim/vendor/SFML/bin/lib/Release/sfml-graphics-s.lib"
  )

# Import target "SFML::System" for configuration "MinSizeRel"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "D:/Git Projects/biosim/vendor/SFML/bin/lib/MinSizeRel/sfml-system-s.lib"
  )

# Import target "SFML::Main" for configuration "MinSizeRel"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "D:/Git Projects/biosim/vendor/SFML/bin/lib/MinSizeRel/sfml-main.lib"
  )

# Import target "SFML::Window" for configuration "MinSizeRel"
set_property(TARGET SFML::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(SFML::Window PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "D:/Git Projects/biosim/vendor/SFML/bin/lib/MinSizeRel/sfml-window-s.lib"
  )

# Import target "SFML::Graphics" for configuration "MinSizeRel"
set_property(TARGET SFML::Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(SFML::Graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "D:/Git Projects/biosim/vendor/SFML/bin/lib/MinSizeRel/sfml-graphics-s.lib"
  )

# Import target "SFML::System" for configuration "RelWithDebInfo"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "D:/Git Projects/biosim/vendor/SFML/bin/lib/RelWithDebInfo/sfml-system-s.lib"
  )

# Import target "SFML::Main" for configuration "RelWithDebInfo"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "D:/Git Projects/biosim/vendor/SFML/bin/lib/RelWithDebInfo/sfml-main.lib"
  )

# Import target "SFML::Window" for configuration "RelWithDebInfo"
set_property(TARGET SFML::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::Window PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "D:/Git Projects/biosim/vendor/SFML/bin/lib/RelWithDebInfo/sfml-window-s.lib"
  )

# Import target "SFML::Graphics" for configuration "RelWithDebInfo"
set_property(TARGET SFML::Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::Graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "D:/Git Projects/biosim/vendor/SFML/bin/lib/RelWithDebInfo/sfml-graphics-s.lib"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)

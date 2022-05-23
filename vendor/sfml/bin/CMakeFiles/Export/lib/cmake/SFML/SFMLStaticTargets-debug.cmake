#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFML::System" for configuration "Debug"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sfml-system-s-d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::System )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::System "${_IMPORT_PREFIX}/lib/sfml-system-s-d.lib" )

# Import target "SFML::Main" for configuration "Debug"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sfml-main-d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Main )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Main "${_IMPORT_PREFIX}/lib/sfml-main-d.lib" )

# Import target "SFML::Window" for configuration "Debug"
set_property(TARGET SFML::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::Window PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sfml-window-s-d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Window )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Window "${_IMPORT_PREFIX}/lib/sfml-window-s-d.lib" )

# Import target "SFML::Graphics" for configuration "Debug"
set_property(TARGET SFML::Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(SFML::Graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sfml-graphics-s-d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Graphics )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Graphics "${_IMPORT_PREFIX}/lib/sfml-graphics-s-d.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

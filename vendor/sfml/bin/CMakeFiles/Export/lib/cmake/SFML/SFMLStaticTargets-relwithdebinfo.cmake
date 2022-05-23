#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SFML::System" for configuration "RelWithDebInfo"
set_property(TARGET SFML::System APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::System PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/sfml-system-s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::System )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::System "${_IMPORT_PREFIX}/lib/sfml-system-s.lib" )

# Import target "SFML::Main" for configuration "RelWithDebInfo"
set_property(TARGET SFML::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/sfml-main.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Main )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Main "${_IMPORT_PREFIX}/lib/sfml-main.lib" )

# Import target "SFML::Window" for configuration "RelWithDebInfo"
set_property(TARGET SFML::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::Window PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/sfml-window-s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Window )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Window "${_IMPORT_PREFIX}/lib/sfml-window-s.lib" )

# Import target "SFML::Graphics" for configuration "RelWithDebInfo"
set_property(TARGET SFML::Graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SFML::Graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/sfml-graphics-s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SFML::Graphics )
list(APPEND _IMPORT_CHECK_FILES_FOR_SFML::Graphics "${_IMPORT_PREFIX}/lib/sfml-graphics-s.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

include(FindPackageHandleStandardArgs)

find_path(PORTAUDIOCPP_INCLUDE_DIR NAMES PortAudioCpp.hxx portaudiocpp/PortAudioCpp.hxx REQUIRED)
find_library(PORTAUDIOCPP_LIBRARY NAMES portaudiocpp libportaudiocpp REQUIRED)

find_package_handle_standard_args(PortAudioCpp
 DEFAULT_MSG
  PORTAUDIOCPP_INCLUDE_DIR
  PORTAUDIOCPP_LIBRARY
)

mark_as_advanced(PORTAUDIOCPP_LIBRARY
                 PORTAUDIOCPP_INCLUDE_DIR)

if(PORTAUDIOCPP_FOUND)
  set(PORTAUDIOCPP_LIBRARIES    ${PORTAUDIOCPP_LIBRARY})
  set(PORTAUDIOCPP_INCLUDE_DIRS ${PORTAUDIOCPP_INCLUDE_DIR})
  include_directories(${PORTAUDIOCPP_INCLUDE_DIR} ${PORTAUDIOCPP_INCLUDE_DIR})
  if (NOT TARGET PortAudio::portaudiocpp)
    add_library(PortAudio::portaudiocpp UNKNOWN IMPORTED)
  endif()
  if (PORTAUDIOCPP_LIBRARY)
    set_property(TARGET PortAudio::portaudiocpp APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
    )
    set_target_properties(PortAudio::portaudiocpp PROPERTIES
      IMPORTED_LOCATION_RELEASE "${PORTAUDIOCPP_LIBRARY}"
    )
  endif()
  #include(SetupRtAudio)
  message("PORTAUDIOCPP FOUND")
  message("PORTAUDIOCPP_LIBRARIES: ${PORTAUDIOCPP_LIBRARIES}")
  message("PORTAUDIOCPP_INCLUDE_DIRS: ${PORTAUDIOCPP_INCLUDE_DIRS}")
else()
  message("PORTAUDIOCPP NOT FOUND")
endif()



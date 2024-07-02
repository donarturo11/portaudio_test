include(FindPackageHandleStandardArgs)

find_path(PORTAUDIO_INCLUDE_DIR NAMES portaudio.h portaudio/portaudio.h REQUIRED)
find_library(PORTAUDIO_LIBRARY NAMES portaudio libportaudio REQUIRED)

find_package_handle_standard_args(PortAudio
 DEFAULT_MSG
  PORTAUDIO_INCLUDE_DIR
  PORTAUDIO_LIBRARY
)

mark_as_advanced(PORTAUDIO_LIBRARY
                 PORTAUDIO_INCLUDE_DIR)

if(PORTAUDIO_FOUND)
  set(PORTAUDIO_LIBRARIES    ${PORTAUDIO_LIBRARY})
  set(PORTAUDIO_INCLUDE_DIRS ${PORTAUDIO_INCLUDE_DIR})
  include_directories(${PORTAUDIO_INCLUDE_DIR} ${PORTAUDIOCPP_INCLUDE_DIR})
  if (NOT TARGET PortAudio::portaudio)
    add_library(PortAudio::portaudio UNKNOWN IMPORTED)
  endif()
  if (PORTAUDIO_LIBRARY)
    set_property(TARGET PortAudio::portaudio APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
    )
    set_target_properties(PortAudio::portaudio PROPERTIES
      IMPORTED_LOCATION_RELEASE "${PORTAUDIO_LIBRARY}"
    )
  endif()
  #include(SetupRtAudio)
  message("PORTAUDIO FOUND")
  message("PORTAUDIO_LIBRARIES: ${PORTAUDIO_LIBRARIES}")
  message("PORTAUDIO_INCLUDE_DIRS: ${PORTAUDIO_INCLUDE_DIRS}")
else()
  message("PORTAUDIO NOT FOUND")
endif()



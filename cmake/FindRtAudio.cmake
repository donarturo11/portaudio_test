include(FindPackageHandleStandardArgs)

find_path(RTAUDIO_INCLUDE_DIR NAMES rtaudio/RtAudio.h rtaudio.h REQUIRED)
find_library(RTAUDIO_LIBRARY NAMES rtaudio librtaudio REQUIRED)

find_package_handle_standard_args(RtAudio
 DEFAULT_MSG
  RTAUDIO_INCLUDE_DIR
  RTAUDIO_LIBRARY
)

mark_as_advanced(RTAUDIO_LIBRARY RTAUDIO_INCLUDE_DIR)

if(RTAUDIO_FOUND)
  set(RTAUDIO_LIBRARIES    ${RTAUDIO_LIBRARY})
  set(RTAUDIO_INCLUDE_DIRS ${RTAUDIO_INCLUDE_DIR})
  include_directories(${RTAUDIO_INCLUDE_DIR})
  if (NOT TARGET RtAudio::rtaudio)
    add_library(RtAudio::rtaudio UNKNOWN IMPORTED)
  endif()
  if (RTAUDIO_LIBRARY)
    set_property(TARGET RtAudio::rtaudio APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
    )
    set_target_properties(RtAudio::rtaudio PROPERTIES
      IMPORTED_LOCATION_RELEASE "${RTAUDIO_LIBRARY}"
    )
  endif()
  include(SetupRtAudio)
  message("RTAUDIO FOUND")
  message("RTAUDIO_LIBRARIES: ${RTAUDIO_LIBRARIES}")
  message("RTAUDIO_INCLUDE_DIRS: ${RTAUDIO_INCLUDE_DIRS}")
  
else()
  message("RTAUDIO NOT FOUND")
endif()



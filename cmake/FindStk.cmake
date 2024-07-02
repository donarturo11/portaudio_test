include(FindPackageHandleStandardArgs)
find_path(STK_INCLUDE_DIR stk/Stk.h stk REQUIRED)
find_library(STK_LIBRARY NAMES stk libstk REQUIRED)

find_package_handle_standard_args(Stk
 DEFAULT_MSG
  STK_INCLUDE_DIR
  STK_LIBRARY
)

mark_as_advanced(STK_LIBRARY STK_INCLUDE_DIR)

if(STK_FOUND)
  set(STK_LIBRARIES    ${STK_LIBRARY})
  set(STK_INCLUDE_DIRS ${STK_INCLUDE_DIR})
  include_directories(${STK_INCLUDE_DIR})
  if (NOT TARGET Stk::stk)
    add_library(Stk::stk UNKNOWN IMPORTED)
  endif()
  if (STK_LIBRARY)
    set_property(TARGET Stk::stk APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
    )
    set_target_properties(Stk::stk PROPERTIES
      IMPORTED_LOCATION_RELEASE "${STK_LIBRARY}"
    )
  endif()
  message("STK FOUND")
  message("STK_LIBRARIES: ${STK_LIBRARIES}")
  message("STK_INCLUDE_DIRS: ${STK_INCLUDE_DIRS}")
  
else()
  message("STK NOT FOUND")  
endif()



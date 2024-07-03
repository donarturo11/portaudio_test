cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(pa_buffer_test VERSION 1.0.0 LANGUAGES CXX C)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(cmake-build-type)
OPTION(USE_PORTAUDIOCPP "Enable support for portaudiocpp" OFF)
include_directories(
       ${PROJECT_SOURCE_DIR}/
       ${PROJECT_SOURCE_DIR}/audio
       )

find_package(PortAudio)
if (USE_PORTAUDIOCPP)
find_package(PortAudioCpp)
add_compile_definitions(USE_PORTAUDIOCPP)
endif()
set(PROJECT_SOURCES
    audio/AudioEngine.cpp
    main.cpp
    )
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

target_link_libraries(${PROJECT_NAME} PRIVATE PortAudio::portaudio)
if (USE_PORTAUDIOCPP)
target_link_libraries(${PROJECT_NAME} PRIVATE PortAudio::portaudiocpp)
endif()

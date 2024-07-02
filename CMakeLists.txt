cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(pa_buffer_test VERSION 1.0.0 LANGUAGES CXX C)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
#set(kissfft_DIR kissfft/lib/cmake/kissfft)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(
       ${PROJECT_SOURCE_DIR}/
       ${PROJECT_SOURCE_DIR}/audio
       )

find_package(Boost)
find_package(PortAudio)
find_package(PortAudioCpp)
#find_package(FFTW3)
#find_package(kissfft)
if (Boost_FOUND)
    set(PROJECT_SOURCES
        audio/AudioEngine.cpp
        main.cpp
        )
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE PortAudio::portaudio)
target_link_libraries(${PROJECT_NAME} PRIVATE PortAudio::portaudiocpp)
#target_link_libraries(${PROJECT_NAME} PRIVATE kissfft::kissfft-float)

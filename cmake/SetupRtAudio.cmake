if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set (LINUX 1)
else()
    set (LINUX 0)
endif()

set(ENABLE_WASAPI 1)
set(ENABLE_DS 0)

if(LINUX)
	find_library(JACK_LIBRARY jack) # find_package(JACK) # TODO: NEED FindJACK.cmake
	if(JACK_LIBRARY)
		message("Jack API found: ${JACK_LIBRARY}")
		link_libraries(${JACK_LIBRARY})
		add_compile_definitions(__UNIX_JACK__)
	else()
		message(WARNING "JACK support requires the jack library!")
	endif()
endif()

message("${CMAKE_SYSTEM_NAME}")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

if(LINUX)
	# TODO: Finish Linux configuration, include different audio API supports
	#==============    LINUX       ================#
	message("Linux DETECTED!")
	if(ENABLE_ALSA)
		find_package(ALSA REQUIRED)
		if(ALSA_FOUND)
			include_directories(${ALSA_INCLUDE_DIRS})
			link_libraries(${ALSA_LIBRARIES})
			add_compile_definitions(__LINUX_ALSA__)
		endif()
	endif()
elseif(APPLE)
	#==============    MAC OS    ================#
	message("Machintosh DETECTED!")
	find_package(CoreAudio REQUIRED)
	include_directories(${COREAUDIO_INCLUDE_DIRS})
	add_compile_definitions(__MACOSX_CORE__)
	link_libraries(${COREAUDIO_LIBRARY} ${COREAUDIO_FOUNDATION} ${COREAUDIO_MIDI})
elseif(WIN32)
	# TODO: MORE SUPPORT (e.g., MSYS)?
	# Tested under MSYS2 with Mingw64 toolchain
	#==============      WINDOWS      ================#
	message("Windows DETECTED!")

	link_libraries(winmm ole32 wsock32)
	add_compile_definitions(__WINDOWS_MM__)
	
	if(ENABLE_WASAPI)
		message("ENABLING WASAPI")
		link_libraries(mfuuid mfplat wmcodecdspuuid ksuser)
		add_compile_definitions(__WINDOWS_WASAPI__)
	endif()
	
	if(ENABLE_DS)
		message("ENABLING Directsound")
		link_libraries(dsound)
		add_compile_definitions(__WINDOWS_DS__)
	endif()
else()
	message("CMAKE_SYSTEM_NAME:" ${CMAKE_SYSTEM_NAME})
	message(FATAL_ERROR "Unknown system type for realtime support.")
endif()


include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(NOT IS_BIG_ENDIAN)
  add_compile_definitions(__LITTLE_ENDIAN__)
endif()

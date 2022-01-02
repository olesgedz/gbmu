set(CMAKE_CXX_EXTENSIONS ON)
set(CXX_STANDARD 20)
if(NOT WIN32)
set(CMAKE_CXX_FLAGS  -std=c++20)
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
	# Default debug flags are OK
	set(CMAKE_CXX_FLAGS_RELEASE "{CMAKE_CXX_FLAGS_RELEASE} /O2")
endif()
set(MAIN_SOURCES main.cc)
include_directories(${PROJECT_SOURCE_DIR}/extern/imgui;${PROJECT_SOURCE_DIR}/extern/imgui/backends;${PROJECT_SOURCE_DIR}/extern/imgui-filebrowser)
include_directories(${PROJECT_SOURCE_DIR}/lib)
message("message libs  ${SDL2_LIBRARIES}")
message("message includes: ${SDL2_INCLUDE_DIRS}")
#set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(gbemu ${MAIN_SOURCES})
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(gbemu lib)
if(WIN32)
	target_link_libraries(gbemu SDL2main SDL2 gl3w)
elseif (UNIX AND NOT APPLE)
target_link_libraries(gbemu SDL2main SDL2 gl3w)
else(APPLE)
	target_link_libraries(gbemu  ${SDL2_LIBRARIES} gl3w)
endif()


	add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2d.dll
	$<TARGET_FILE_DIR:${PROJECT_NAME}>
	VERBATIM)
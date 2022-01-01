set(MAIN_SOURCES audio.cc cpu.cc dromaius.cc graphics.cc gui.cc imgui_impl_opengl3.cc imgui_impl_sdl.cc
		input.cc main.cc memory.cc)
include_directories(${PROJECT_SOURCE_DIR}/extern/imgui;${PROJECT_SOURCE_DIR}/extern/imgui/backends;${CMAKE_BINARY_DIR}/extern/gl3w/include;
		${PROJECT_SOURCE_DIR}/extern/imgui-filebrowser)
message("message libs  ${SDL2_LIBRARIES}")
message("message includes: ${SDL2_INCLUDE_DIRS}")
#set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(gbemu ${MAIN_SOURCES})
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(gbemu lib)
target_link_libraries(gbemu SDL2main SDL2 gl3w)


add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2d.dll
	$<TARGET_FILE_DIR:${PROJECT_NAME}>
	VERBATIM)
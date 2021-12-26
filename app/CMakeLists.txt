set(MAIN_SOURCES emu.cpp ${GL3W_SRC})
message("message libs  ${SDL2_LIBRARIES}")
message("message includes: ${SDL2_INCLUDE_DIRS}")
#set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(gbemu ${MAIN_SOURCES})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/extern/gl3w/include)
target_link_libraries(gbemu lib)
target_link_libraries(gbemu SDL2main SDL2)


add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2d.dll
	$<TARGET_FILE_DIR:${PROJECT_NAME}>
	VERBATIM)
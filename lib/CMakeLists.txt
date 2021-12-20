set(sources  Application.cpp)

include_directories(${PROJECT_SOURCE_DIR}/extern/imgui;${PROJECT_SOURCE_DIR}/extern/imgui/backends;${CMAKE_BINARY_DIR}/extern/gl3w/include)
add_library(lib ${sources} ../extern/imgui/backends/imgui_impl_opengl3.cpp ../extern/imgui/backends/imgui_impl_sdl.cpp ../extern/imgui/imgui.cpp
	../extern/imgui/imgui_widgets.cpp ../extern/imgui/imgui_tables.cpp ../extern/imgui/imgui_draw.cpp)
target_include_directories(lib PUBLIC ${PROJECT_SOURCE_DIR}/lib)
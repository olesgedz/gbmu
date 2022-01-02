#set(sources  Application.cpp)
set(CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
add_compile_options(-Wc++11-extensions -std=c++20)
include_directories(${PROJECT_SOURCE_DIR}/extern/imgui;${PROJECT_SOURCE_DIR}/extern/imgui/backends;${CMAKE_BINARY_DIR}/extern/gl3w/include;
		${PROJECT_SOURCE_DIR}/extern/imgui-filebrowser)
add_library(lib audio.cc cpu.cc dromaius.cc graphics.cc gui.cc imgui_impl_opengl3.cc imgui_impl_sdl.cc
		input.cc  memory.cc ../extern/imgui/imgui.cpp
	../extern/imgui/imgui_widgets.cpp ../extern/imgui/imgui_tables.cpp ../extern/imgui/imgui_draw.cpp)
target_include_directories(lib PUBLIC ${PROJECT_SOURCE_DIR}/lib)
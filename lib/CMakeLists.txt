#set(sources  Application.cpp)
set(CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
if(NOT WIN32)
#add_compile_options(-Wc++11-extensions)
add_compile_options(-std=c++20)
elseif(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
		# Default debug flags are OK
		set(CMAKE_CXX_FLAGS_RELEASE "{CMAKE_CXX_FLAGS_RELEASE} /O2")
endif()
include_directories(${PROJECT_SOURCE_DIR}/extern/imgui;${PROJECT_SOURCE_DIR}/extern/imgui/backends;${CMAKE_BINARY_DIR}/extern/gl3w/include;
		${PROJECT_SOURCE_DIR}/extern/imgui-filebrowser)
add_library(lib audio.cc cpu.cc dromaius.cc graphics.cc gui.cc ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_sdl.cpp #imgui_impl_opengl3.cc imgui_impl_sdl.cc
		input.cc  memory.cc ../extern/imgui/imgui.cpp
	../extern/imgui/imgui_widgets.cpp ../extern/imgui/imgui_tables.cpp ../extern/imgui/imgui_draw.cpp)
target_include_directories(lib PUBLIC ${PROJECT_SOURCE_DIR}/lib)
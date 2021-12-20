project(gbemu C CXX) #LANGUAGES CXX
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
#if(NOT CMAKE_CXX_EXTENSIONS)
#	set(CMAKE_CXX_EXTENSIONS OFF)
#endif()
#set(CXX_STANDARD 17)
add_compile_options(
#	-Werror
#
#	-Wall
#	-Wextra
#	-Wpedantic
#
#	-Wcast-align
#	-Wcast-qual
#	-Wconversion
#	-Wctor-dtor-privacy
#	-Wenum-compare
#	-Wfloat-equal
#	-Wnon-virtual-dtor
#	-Wold-style-cast
#	-Woverloaded-virtual
#	-Wredundant-decls
#	-Wsign-conversion
#	-Wsign-promo
)
#add_subdirectory(src)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2")
set(CMAKE_BUILD_TYPE Debug)
if (WIN32)
#	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W3 /MTd")
	add_subdirectory( extern/SDL )
##	add_subdirectory(extern/SDL)
	include_directories("${PROJECT_SOURCE_DIR}/extern/sdl2_win/include;${PROJECT_SOURCE_DIR}/extern/sdl2_win/include/SDL2")
#
#set(SDL2_DIR "${PROJECT_SOURCE_DIR}/extern/sdl2_win")
#set(SDL2_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/extern/sdl2_win/include/SDL2")
#if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
#	set(SDL2_LIBRARIES "${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2.lib;${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2main.lib")
#else ()
#	set(SDL2_LIBRARIES "${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x86/SDL2.lib;${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x86/SDL2main.lib")
#endif ()

elseif(UNIX)
#	find_package(SDL2 REQUIRED)
endif()
include_directories(${SDL2_INCLUDE_DIRS})
add_subdirectory(extern/gl3w)
add_subdirectory(lib)
add_subdirectory(app)
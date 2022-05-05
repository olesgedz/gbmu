project(gbemu CXX C) #LANGUAGES CXX
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
message("S" ${CMAKE_CXX_COMPILER})
##if(NOT CMAKE_CXX_EXTENSIONS)
##	set(CMAKE_CXX_EXTENSIONS ON)
##endif()
##set(CMAKE_CXX_EXTENSIONS ON)
##set(CXX_STANDARD 20)
set(CMP0072 NEW)
if (UNIX)
add_compile_options(
#		-Wc++11-extensions
#			-Werror
#
#	-Wall
	-Wextra
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
#	-Wsi gn-promo
)
endif()
##add_subdirectory(src)
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2")
set(CMAKE_BUILD_TYPE Debug)
# set(OpenGL_GL_PREFERENCE GLVND)



if (WIN32)
#	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W3 /MTd")
	add_subdirectory( extern/SDL )
##	add_subdirectory(extern/SDL)
	include_directories("${PROJECT_SOURCE_DIR}/extern/SDL/include;${PROJECT_SOURCE_DIR}/extern/sdl2_win/SDL/SDL2")
#
set(SDL2_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/extern/SDL/include/SDL2")
set(SDL2_LIBRARIES "${CMAKE_BINARY_DIR}/extern/SDL/libSDL2maind.lib")
#if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
#	set(SDL2_LIBRARIES "${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2.lib;${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x64/SDL2main.lib")
#else ()
#	set(SDL2_LIBRARIES "${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x86/SDL2.lib;${PROJECT_SOURCE_DIR}/extern/sdl2_win/lib/x86/SDL2main.lib")
#endif ()
set(OpenGL_GL_PREFERENCE GLVND)
elseif(UNIX AND NOT APPLE)
#	find_package(SDL2 REQUIRED)
	set(SDL2_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/extern/SDL/include/")
	include_directories("${PROJECT_SOURCE_DIR}/extern/SDL/include")
	set(SDL2_LIBRARIES "${CMAKE_BINARY_DIR}/extern/SDL/libSDL2maind.a;${CMAKE_BINARY_DIR}/extern/SDL/libSDL2d.a")
	add_subdirectory(extern/SDL)
elseif(APPLE)
#	find_package(SDL2 REQUIRED)
##	add_subdirectory(extern/SDL)
#find_package(SDL2 REQUIRED)
set(SDL2_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/extern/SDL/include/")
include_directories("${PROJECT_SOURCE_DIR}/extern/SDL/include")
set(SDL2_LIBRARIES "${CMAKE_BINARY_DIR}/extern/SDL/libSDL2maind.a;${CMAKE_BINARY_DIR}/extern/SDL/libSDL2d.a")
add_subdirectory(extern/SDL)
endif()
#execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/extern/gl3w WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/gl3w)
# set(GL3W_DIR "${PROJECT_SOURCE_DIR}/extern/gl3w/cmake/")
# find_package(GL3W REQUIRED)
set(GL3W_ROOT "${CMAKE_SOURCE_DIR}/extern/gl3w")
set(GL3W_INC "${GL3W_ROOT}/include")
set(GL3W_SRC "${GL3W_ROOT}/src/gl3w.c")

if (NOT EXISTS ${GL3W_SRC})
message(STATUS "Generating gl3w source files")
execute_process(COMMAND python gl3w_gen.py --root ${PROJECT_BINARY_DIR}/extern/gl3w
WORKING_DIRECTORY ${GL3W_ROOT})
endif()

include_directories(${SDL2_INCLUDE_DIRS})
add_subdirectory(extern/gl3w)
add_subdirectory(lib)
add_subdirectory(app)
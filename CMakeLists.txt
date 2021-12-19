project(gbemu) #LANGUAGES CXX
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
#if(NOT CMAKE_CXX_EXTENSIONS)
#	set(CMAKE_CXX_EXTENSIONS OFF)
#endif()
set(CXX_STANDARD 17)
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

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_BUILD_TYPE Debug)


find_package(SDL2 REQUIRED)

#add_subdirectory(extern/SDL)
#include_directories(extern/SDL/include)
#set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/extern/SDL/include")
#set(SDL2_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/extern/SDL/libSDL2d.a;${CMAKE_CURRENT_BINARY_DIR}/extern/SDL/libSDL2maind.a")
message("message ${SDL2_LIBRARIES}")

add_subdirectory(app)

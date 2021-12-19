set(sources  Application.cpp)
add_library(lib ${sources})
target_include_directories(lib PUBLIC ${PROJECT_SOURCE_DIR}/lib)
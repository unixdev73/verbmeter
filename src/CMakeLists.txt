if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_compile_options(-Wall -Wextra -Wpedantic -O0 -g)
endif()

add_subdirectory(query)
add_subdirectory(algo)

add_executable(verbmhist histogram.cpp)
target_link_libraries(verbmhist query algo)

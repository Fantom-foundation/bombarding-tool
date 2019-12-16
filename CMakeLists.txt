cmake_minimum_required (VERSION 3.0.0)
project (log-analyzer CXX)

set (CMAKE_CXX_STANDARD 14)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"
)
    if (${CMAKE_BUILD_TYPE} MATCHES "Release|MinSizeRel")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    endif ()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wcast-qual -Wshadow -Winit-self")
endif ()

add_executable (main "main.cpp")

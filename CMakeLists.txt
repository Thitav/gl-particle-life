cmake_minimum_required(VERSION 3.5.0)
project(gl-particles VERSION 0.1.0 LANGUAGES C)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c" "src/**/*.c" "src/*.h" "src/**/*.h")

add_subdirectory("lib/glfw" EXCLUDE_FROM_ALL)

add_executable(gl-particles "lib/glad/src/gl.c " ${SOURCES}
        src/simulation/group.c
        src/simulation/group.h
        src/simulation/error.h
        src/gl/types.h)
target_include_directories(gl-particles PRIVATE "src/" PUBLIC "lib/glad/include")
target_link_libraries(gl-particles PRIVATE glfw)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

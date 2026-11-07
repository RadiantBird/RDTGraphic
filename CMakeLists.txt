cmake_minimum_required(VERSION 3.14)
project(DrawTexts C CXX)

set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCES src/*.cpp)

add_executable(dtxt ${SOURCES})

file(GLOB_RECURSE INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
)

list(FILTER INCLUDE_DIRS INCLUDE REGEX "/[^.]+$")

target_include_directories(dtxt PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${INCLUDE_DIRS}
)

# libtess2
set(LIBTESS2_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/external/libtess2-1.0.2"
)

file(GLOB LIBTESS2_SOURCES
    "${LIBTESS2_DIR}/Source/*.c"
)

add_library(libtess2 STATIC ${LIBTESS2_SOURCES})

target_include_directories(libtess2 PUBLIC
    "${LIBTESS2_DIR}/Include"
)

target_link_directories(dtxt PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(dtxt PRIVATE GLEW_STATIC)

target_link_libraries(dtxt PRIVATE
    libtess2
    opengl32
    gdi32
    winmm
    glew32s
    glfw3
)

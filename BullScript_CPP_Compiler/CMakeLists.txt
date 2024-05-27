cmake_minimum_required(VERSION 3.27)
project(bullscript_cpp_compiler)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

set(SOURCES
    main.cpp
    util.cpp
)

add_executable(bullscript_cpp_compiler ${SOURCES})

install(TARGETS bullscript_cpp_compiler RUNTIME DESTINATION bin)

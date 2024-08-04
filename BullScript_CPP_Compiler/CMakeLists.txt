cmake_minimum_required(VERSION 3.27)
project(bull_cpp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

set(SOURCES main.cpp Util/util.cpp Basm/basmCompiler.cpp)

add_executable(bull_cpp ${SOURCES})

install(TARGETS bull_cpp RUNTIME DESTINATION bin)

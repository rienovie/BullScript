cmake_minimum_required(VERSION 3.27)
project(bull_cpp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

find_package(SQLite3 REQUIRED)

set(SOURCES main.cpp Util/util.cpp Basm/basmCompiler.cpp logger.cpp)

add_executable(bull_cpp ${SOURCES})

include_directories(${SQLite3_INCLUDE_DIRS})

target_link_libraries(bull_cpp ${SQLite3_LIBRARIES})

install(TARGETS bull_cpp RUNTIME DESTINATION bin)

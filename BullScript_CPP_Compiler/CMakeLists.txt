cmake_minimum_required(VERSION 3.27)
project(bull_cpp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")

list(FILTER SOURCES EXCLUDE REGEX "(playground|build)")

add_executable(bull_cpp ${SOURCES})

include_directories(${SQLite3_INCLUDE_DIRS})

target_link_libraries(bull_cpp ${SQLite3_LIBRARIES})

install(TARGETS bull_cpp RUNTIME DESTINATION bin)
install(FILES Basm/basmTranslations.db
        DESTINATION ${CMAKE_INSTALL_PREFIX}/Basm/)

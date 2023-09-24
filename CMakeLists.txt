cmake_minimum_required(VERSION 2.8)
project(ASCIICAM)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ASCIICAM ASCIICAM.cpp)

target_link_libraries(ASCIICAM ${OpenCV_LIBS})

find_library(NCURSES_LIBRARY ncurses)
target_link_libraries(ASCIICAM ${NCURSES_LIBRARY})


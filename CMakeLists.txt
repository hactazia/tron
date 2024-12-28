cmake_minimum_required(VERSION 3.10)
project(untitled C)

set(CMAKE_C_STANDARD 11)

# Find the NCURSES package
find_package(Curses REQUIRED)

# Include the NCURSES header files
include_directories(${CURSES_INCLUDE_DIR})

# Add the executable
add_executable(untitled main.c tron.c vue_sdl.c vue_ncurses.c utils.c
        ncurses.h
        controller.c
        model.c)

# Link the NCURSES library
target_link_libraries(untitled ${CURSES_LIBRARIES})

# Link the math library
target_link_libraries(untitled m)
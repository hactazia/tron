cmake_minimum_required(VERSION 3.10)
project(tron C)

set(CMAKE_C_STANDARD 11)

# Find the NCURSES package
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Find the SDL2 library
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Use pkg-config to find the SDL2_ttf library
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
include_directories(${SDL2_TTF_INCLUDE_DIRS})
link_directories(${SDL2_TTF_LIBRARY_DIRS})

# Add the executable
set(
        SOURCE_FILES
        main.c
        tron.c
        vue_sdl.c
        vue_ncurses.c
        utils.c
        controller.c
        model.c
)

add_executable(tron ${SOURCE_FILES})

# Link the NCURSES library
target_link_libraries(tron ${CURSES_LIBRARIES})

# Link the SDL2 library
target_link_libraries(tron ${SDL2_LIBRARIES})

# Link the SDL2_ttf library
target_link_libraries(tron ${SDL2_TTF_LIBRARIES})

# Link the math library
target_link_libraries(tron m)

# Copy assets directory to the build directory
add_custom_command(TARGET tron POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:tron>/assets
)
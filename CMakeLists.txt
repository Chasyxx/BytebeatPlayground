cmake_minimum_required(VERSION 3.27)
project(BytebeatPlayground)

# Find SDL2 using CMake's find_package
find_package(SDL2 REQUIRED)

add_executable(bytebeatPlayground main.cpp)
add_executable(fonttest fonttest.cpp)

set(FONT_I_FILE ${CMAKE_BINARY_DIR}/font.ipp)

# Use execute_process to get SDL2 flags and libs
execute_process(COMMAND sdl2-config --cflags OUTPUT_VARIABLE SDL_CFLAGS)
execute_process(COMMAND sdl2-config --libs OUTPUT_VARIABLE SDL_LIBS)
execute_process(COMMAND perl font.pl OUTPUT_VARIABLE FONT_I)
file(WRITE ${FONT_I_FILE} "${FONT_I}")

# Strip trailing newline characters
string(STRIP "${SDL_CFLAGS}" SDL_CFLAGS)
string(STRIP "${SDL_LIBS}" SDL_LIBS)

# Add the SDL2 flags and libs to the compile options
target_compile_options(bytebeatPlayground PRIVATE -Wall -Wextra ${SDL_CFLAGS})
target_link_libraries(bytebeatPlayground PRIVATE ${SDL_LIBS})
target_compile_options(fonttest PRIVATE -Wall -Wextra ${SDL_CFLAGS})
target_link_libraries(fonttest PRIVATE ${SDL_LIBS})

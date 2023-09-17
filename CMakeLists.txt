cmake_minimum_required(VERSION 3.0)

project(ParCeiro)

# Define source files
file(GLOB MAIN_SOURCES "src/main.c")
file(GLOB MODULE_SOURCES "src/modules/*.c")

# Include directories
include_directories(headers)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# Define the executable
add_executable(parc ${MAIN_SOURCES} ${MODULE_SOURCES})


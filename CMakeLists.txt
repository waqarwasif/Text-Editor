cmake_minimum_required(VERSION 3.16)

# Project Name and Language
project(TextEditor LANGUAGES CXX)

# ---- Standards ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---- Include directories ----
# This ensures your modular architecture (classes and functions) can find the header files
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# ---- Collect all C++ source files ----
# Automatically grabs all .cpp files in the src/ directory and its subfolders
file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# ---- Create executable ----
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

set(TOOLS_DST_DIR "${CMAKE_BINARY_DIR}")
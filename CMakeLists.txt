cmake_minimum_required(VERSION 3.16)

project(TextEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Force static linking so MSYS2 doesn't require background DLLs to run
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE CORE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/DataStructures/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/Core/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/Utils/*.cpp"
)

add_executable(${PROJECT_NAME} src/main.cpp ${CORE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cpp")

add_executable(TextEditorTests ${TEST_FILES} ${CORE_FILES})
target_include_directories(TextEditorTests PRIVATE ${PROJECT_SOURCE_DIR}/src)

cmake_minimum_required(VERSION 3.16)

project(TextEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_options(/W4)
else()
    # Static linking for standalone binary portability
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    add_compile_options(-Wall -Wextra)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE CORE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/DataStructures/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/Core/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/Utils/*.cpp"
)

add_executable(${PROJECT_NAME} src/main.cpp ${CORE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Test suite configuration
enable_testing()
file(GLOB TEST_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cpp")
foreach(test_src IN LISTS TEST_FILES)
    get_filename_component(test_name ${test_src} NAME_WE)

    # Skip files without main()
    file(READ ${test_src} test_text)
    string(FIND "${test_text}" "main(" has_main)
    if(has_main EQUAL -1)
        message(STATUS "Skipping ${test_name}: no main() yet")
        continue()
    endif()

    # Re-run CMake when this file's content changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${test_src})

    add_executable(${test_name} ${test_src} ${CORE_FILES})
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
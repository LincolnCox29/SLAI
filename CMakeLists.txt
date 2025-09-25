cmake_minimum_required(VERSION 3.10)
project(SLAI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(includes)

set(SOURCES
    src/interpreter.cpp
    src/token.cpp
    src/main.cpp
)

# Create executable
add_executable(slai ${SOURCES})

# Platform-specific compiler options
if(MSVC)
    # Microsoft Visual C++ compiler
    target_compile_options(slai PRIVATE
        /W4          # High warning level
        /permissive- # Standards conformance
        /utf-8       # UTF-8 source files
    )
else()
    # GCC/Clang compilers
    target_compile_options(slai PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

target_compile_definitions(slai PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

set_target_properties(slai PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
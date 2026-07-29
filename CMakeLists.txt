cmake_minimum_required(VERSION 3.20)

project(quark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Sources
file(GLOB SRC_FILES
    src/*.cpp
    src/passes/*.cpp
)

# Executable
add_executable(quark ${SRC_FILES})

# Include directories
target_include_directories(quark PRIVATE
    include
    tabulate/include
    ${LLVM_INCLUDE_DIRS}
)

# LLVM compile definitions
target_compile_definitions(quark PRIVATE
    ${LLVM_DEFINITIONS}
)

# Compiler warnings
target_compile_options(quark PRIVATE
    -Wall
)

# Link against LLVM
target_link_libraries(quark PRIVATE LLVM)

# Output directory
set_target_properties(quark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

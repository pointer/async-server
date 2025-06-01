cmake_minimum_required(VERSION 3.5..3.29)
project("Asynchronous Programming in C++")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Boost
find_package(Boost 1.88.0 REQUIRED COMPONENTS thread system container)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost 1.88.0 is required but not found.")
endif()

# Include Boost directories
include_directories(${Boost_INCLUDE_DIRS})

# Find GoogleTest
enable_testing()
find_package(GTest REQUIRED)
if(NOT GTest_FOUND)
    message(FATAL_ERROR "GoogleTest library is required but not found.")
endif()
include(GoogleTest)

# Include GoogleTest directories
include_directories(${GTest_INCLUDE_DIRS})

# Add {fmt} library
find_package(fmt)
if(NOT fmt_FOUND)
    message(FATAL_ERROR "fmt library is required but not found.")
endif()

# Linker flags
if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -Wall -Wextra -pedantic")
endif()

# Set the output directory for binaries
set(BINARY_DIR ${CMAKE_BINARY_DIR}/bin)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}/Output)

# Create executable for each cpp file
file(GLOB SOURCES "*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    message(${EXECUTABLE_NAME})
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
    target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES})
endforeach()


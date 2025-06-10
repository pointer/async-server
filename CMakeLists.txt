cmake_minimum_required(VERSION 3.5..3.29)
project("Asynchronous Programming in C++")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Set the C++ compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
# Set the C++ compiler flags for warnings
if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable")
endif()

# Linker flags
if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -Wall -Wextra -pedantic")
endif()

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

find_package(nlohmann_json 3.11.3 REQUIRED)
if(NOT nlohmann_json_FOUND)
    message(FATAL_ERROR "nlohmann_json library is required but not found.")
endif()
# Include nlohmann_json directories
include_directories(${nlohmann_json_INCLUDE_DIRS})
# Include GoogleTest directories
include_directories(${GTest_INCLUDE_DIRS})

# Add {fmt} library
find_package(fmt)
if(NOT fmt_FOUND)
    message(FATAL_ERROR "fmt library is required but not found.")
endif()

set(JSON_Install OFF CACHE INTERNAL "")

# add_subdirectory($PROJ/thirdparty/nlohmann-json/nlohmann-json)

# include_directories(../third-party/json)
#     ${CMAKE_BINARY_DIR}/third-party/json)
# if(NOT nlohmann_json_FOUND)
#     message(FATAL_ERROR "nlohmann_json library is required but not found.")
# endif()

# # Include nlohmann_json directories
# include_directories(${nlohmann_json_INCLUDE_DIRS})

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
    target_link_libraries(${EXECUTABLE_NAME} GTest::gtest GTest::gtest_main)
    target_link_libraries(${EXECUTABLE_NAME} fmt::fmt)
    target_link_libraries(${EXECUTABLE_NAME} nlohmann_json::nlohmann_json)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )
endforeach()


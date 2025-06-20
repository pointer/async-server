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

# Add yescrypt library
add_library(yescrypt STATIC
    yescrypt-1.1.0/yescrypt-common.c
    yescrypt-1.1.0/yescrypt-opt.c
    yescrypt-1.1.0/yescrypt-platform.c
    yescrypt-1.1.0/yescrypt-ref.c
    yescrypt-1.1.0/sha256.c
    yescrypt-1.1.0/insecure_memzero.c
)

target_include_directories(yescrypt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/yescrypt-1.1.0)
# Set the output directory for yescrypt library
set_target_properties(yescrypt PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
# Set the output directory for binaries
set(BINARY_DIR ${CMAKE_BINARY_DIR}/bin)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}/Output)

# Create executable for async-server that includes all source files
add_executable(async-server
    async-server.cpp
    UserManager.cpp
    SessionManager.cpp
    LogFile.cpp
    user.cpp
)

target_link_libraries(async-server
    ${Boost_LIBRARIES}
    GTest::gtest GTest::gtest_main
    fmt::fmt
    nlohmann_json::nlohmann_json
    yescrypt
)

set_target_properties(async-server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)


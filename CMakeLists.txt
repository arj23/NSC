cmake_minimum_required(VERSION 3.10)

# Project Name
project(MessageQueueProject)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(${PROJECT_SOURCE_DIR})

# Locate Google Test (if installed on system)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add the POSIX Real-Time library if available
if(UNIX AND NOT APPLE)
    set(LIBS ${LIBS} rt)
endif()

# Source files for the MessageQueue library
set(SOURCE_FILES ipc_message_queue.cpp)

# Add the main library (MessageQueue)
add_library(MessageQueueLib ${SOURCE_FILES})

# Test executable with Google Test
add_executable(test_message_queue tests/test_ipc_message_queue.cpp)
target_link_libraries(test_message_queue MessageQueueLib ${GTEST_LIBRARIES} pthread ${LIBS})

# Enable testing and add the Google Test executable as a test
enable_testing()
add_test(NAME MessageQueueTests COMMAND test_message_queue)

CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(MyProject)

# Set C++ standard
SET(CMAKE_CXX_STANDARD 11)

# Add Google Test as a subdirectory
add_subdirectory(third_party/googletest)

# Define the main executable
add_executable(my_project src/main.cpp src/my_class.cpp)

# Define the test executable
add_executable(test_my_class tests/test_my_class.cpp)

# Link Google Test and pthread (on Linux) to the test executable
target_link_libraries(test_my_class gtest gtest_main pthread)

# Optionally link Google Test to the main executable (if needed)
target_link_libraries(my_project gtest gtest_main)

# Enable testing and add a test
enable_testing()
add_test(NAME MyTest COMMAND test_my_class)

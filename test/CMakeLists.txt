CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(MyProject)

# Set C++ standard
SET(CMAKE_CXX_STANDARD 11)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        main  # Or use a specific release tag
  )

# Make sure that the Google Test source code is downloaded
FetchContent_MakeAvailable(googletest)

# Define the test executable
add_executable(http_test ${CMAKE_SOURCE_DIR}/test/http_test.cpp)

# Link Google Test and pthread (on Linux) to the test executable
target_link_libraries(http_test gtest gtest_main pthread)

# Enable testing and add a test
enable_testing()
add_test(NAME MyTest COMMAND test_my_class)

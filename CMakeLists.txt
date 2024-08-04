cmake_minimum_required(VERSION 3.10)
project(server VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE CLIENT_SOURCES "client/*.cpp")
message("the sources are ${SOURCES}")

# Add the executable target
add_executable(server ${SOURCES})
add_executable(client ${CLIENT_SOURCES})

# Add include directories
set(header_path
  ${PROJECT_SOURCE_DIR}/include/net/
  ${PROJECT_SOURCE_DIR}/include/core/
  ${PROJECT_SOURCE_DIR}/include/log/
  )
target_include_directories(server PRIVATE ${header_path})

# Optionally add libraries
# find_package(SomeLibrary REQUIRED)
# target_link_libraries(MyExecutable PRIVATE SomeLibrary::SomeLibrary)


SET(INSTALL_DIR "${CMAKE_SOURCE_DIR}/release")
SET(CMAKE_INSTALL_LIBDIR ${INSTALL_DIR}/lib)
SET(CMAKE_INSTALL_INCLUDEDIR ${INSTALL_DIR}/include)
SET(CMAKE_INSTALL_BINDIR ${INSTALL_DIR}/demo)
install(TARGETS server DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS client DESTINATION ${CMAKE_INSTALL_BINDIR})

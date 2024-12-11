CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(server VERSION 1.0)

# Set C++ standard
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

FILE(GLOB_RECURSE SOURCES "src/*.cpp")
FILE(GLOB_RECURSE CLIENT_SOURCES "client/*.cpp")
message("the sources are ${SOURCES}")
FILE(GLOB_RECURSE 3RD_SOURCES "3rd/*.c")
# Add the executable target
add_executable(server ${SOURCES} ${3RD_SOURCES})
add_executable(client ${CLIENT_SOURCES} ${3RD_SOURCES})

add_definitions(-DLOG_USE_COLOR)
# Add include directories
SET(header_path
    ${PROJECT_SOURCE_DIR}/include/net/ ${PROJECT_SOURCE_DIR}/include/core/
    ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/3rd/log/include/)
target_include_directories(server PRIVATE ${header_path})
target_include_directories(client PRIVATE ${header_path})

# Optionally add libraries find_package(SomeLibrary REQUIRED)
# target_link_libraries(MyExecutable PRIVATE SomeLibrary::SomeLibrary)

SET(INSTALL_DIR "${CMAKE_SOURCE_DIR}/release")
SET(CMAKE_INSTALL_LIBDIR ${INSTALL_DIR}/lib)
SET(CMAKE_INSTALL_INCLUDEDIR ${INSTALL_DIR}/include)
SET(CMAKE_INSTALL_BINDIR ${INSTALL_DIR}/demo)
INSTALL(TARGETS server DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(TARGETS client DESTINATION ${CMAKE_INSTALL_BINDIR})

SET(TEST_ENABLED True)

if(TEST_ENABLED)
  add_subdirectory(test)
endif()

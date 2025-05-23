cmake_minimum_required(VERSION 3.10)
project(server VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "-g")

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE CLIENT_SOURCES "client/*.cpp")
message("the sources are ${SOURCES}")
file(GLOB_RECURSE 3RD_SOURCES "3rd/*.c")
# Add the executable target
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_executable(server ${SOURCES} ${3RD_SOURCES})
endif()
add_executable(client ${CLIENT_SOURCES} ${3RD_SOURCES})

add_definitions(-DLOG_USE_COLOR)
# Add include directories
set(header_path
    ${PROJECT_SOURCE_DIR}/include/net/ ${PROJECT_SOURCE_DIR}/include/core/
    ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/3rd/log/include/)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_include_directories(server PRIVATE ${header_path})
endif()

target_include_directories(client PRIVATE ${header_path})

# Optionally add libraries find_package(SomeLibrary REQUIRED)
# target_link_libraries(MyExecutable PRIVATE SomeLibrary::SomeLibrary)

set(INSTALL_DIR "${CMAKE_SOURCE_DIR}/release")
set(CMAKE_INSTALL_LIBDIR ${INSTALL_DIR}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${INSTALL_DIR}/include)
set(CMAKE_INSTALL_BINDIR ${INSTALL_DIR}/demo)
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  install(TARGETS server DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(TARGETS client DESTINATION ${CMAKE_INSTALL_BINDIR})

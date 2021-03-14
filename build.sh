#!/bin/bash

# Define the project directory and build directory
PROJECT_DIR="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

# Navigate to the build directory
cd "$BUILD_DIR"

# Run CMake to configure the project
cmake "$PROJECT_DIR"

# Check if CMake configuration was successful
if [ $? -ne 0 ]; then
    echo "CMake configuration failed."
    exit 1
fi

# Build the project
cmake --build .
cmake --install .

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

echo "Build completed successfully."

# Optional: Run the executable if needed
# ./MyExecutable


find src -name "*.cpp" -exec clang-format -i {} \;
find include -name "*.cpp" -exec clang-format -i {} \;
find client -name "*.cpp" -exec clang-format -i {} \;
cmake-format -i CMakeLists.txt

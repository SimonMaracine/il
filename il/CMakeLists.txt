cmake_minimum_required(VERSION 3.20)

add_executable(il
    "src/context.cpp"
    "src/context.hpp"
    "src/interpreter.cpp"
    "src/interpreter.hpp"
    "src/main.cpp"
    "src/scanner.cpp"
    "src/scanner.hpp"
    "src/token.hpp"
)

target_include_directories(il PUBLIC "src")

target_compile_features(il PUBLIC cxx_std_17)
set_target_properties(il PROPERTIES CXX_EXTENSIONS OFF)

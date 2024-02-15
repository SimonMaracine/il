cmake_minimum_required(VERSION 3.20)

add_executable(il
    "src/ast_printer.hpp"
    "src/ast.hpp"
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

if(UNIX)
    target_compile_options(il PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(il PRIVATE "/W3")
else()
    message(WARNING "Compiler is not MSVC or platform is not supported, warnings are not enabled")
endif()

target_compile_features(il PRIVATE cxx_std_17)
set_target_properties(il PROPERTIES CXX_EXTENSIONS OFF)

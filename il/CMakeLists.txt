cmake_minimum_required(VERSION 3.20)

add_executable(il
    "src/analyzer.cpp"
    "src/analyzer.hpp"
    "src/ast_printer.cpp"
    "src/ast_printer.hpp"
    "src/ast.hpp"
    "src/builtins.cpp"
    "src/builtins.hpp"
    "src/context.cpp"
    "src/context.hpp"
    "src/environment.cpp"
    "src/environment.hpp"
    "src/il.cpp"
    "src/il.hpp"
    "src/interpreter.cpp"
    "src/interpreter.hpp"
    "src/main.cpp"
    "src/object.cpp"
    "src/object.hpp"
    "src/parser.cpp"
    "src/parser.hpp"
    "src/return.hpp"
    "src/runtime_error.hpp"
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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(il PRIVATE "NDEBUG")
endif()

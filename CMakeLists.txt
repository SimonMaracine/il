cmake_minimum_required(VERSION 3.20)

project(il)

add_executable(il
    "src/main.cpp"
    "src/token.cpp"
    "src/lexer.cpp"
    "src/parser.cpp"
    "src/ast.cpp"
)

target_include_directories(il PUBLIC "src")

target_compile_features(il PUBLIC cxx_std_17)
set_target_properties(il PROPERTIES CXX_EXTENSIONS OFF)

cmake_minimum_required(VERSION 3.20)

project(il)

add_executable(il
    "src/main.cpp"
    "src/lexer.cpp"
)

target_include_directories(il PUBLIC "src")

cmake_minimum_required(VERSION 3.15)

project(lab1 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
add_compile_options(-stdlib=libc++)

include_directories(include)

add_executable(lab1 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

if(MSVC)
    target_compile_options(lab1 PRIVATE /W4)
else()
    target_compile_options(lab1 PRIVATE -Wall -Wextra)
endif()
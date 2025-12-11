cmake_minimum_required(VERSION 3.15)

project(lab7 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
add_compile_options(-stdlib=libc++)

include_directories(include)

# Главный исполняемый файл
add_executable(lab7_main src/main.cpp)

# UDP-сервер
add_executable(udp_server src/udp_server.cpp)

# UDP-клиент
add_executable(udp_client src/udp_client.cpp)

# TCP-сервер
add_executable(tcp_server src/tcp_server.cpp)

# TCP-клиент
add_executable(tcp_client src/tcp_client.cpp)

foreach(target lab7_main udp_server udp_client)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    endif()
endforeach()
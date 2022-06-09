cmake_minimum_required(VERSION 3.10)

project(chess VERSION 0.0.1)

configure_file(config.h.in config.h)

add_executable(chess main.cpp)
target_include_directories(chess PUBLIC "${PROJECT_BINARY_DIR}")
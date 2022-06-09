cmake_minimum_required(VERSION 3.10)

set(CHESS_PROJECT_NAME chess)
set(CHESS_SOURCE_DIR src)
set(CHESS_DIR ${CHESS_SOURCE_DIR}/main.cpp)
set(CHESS_LIBRARIES )

project(${CHESS_PROJECT_NAME} VERSION 0.0.1)

configure_file(config.h.in config.h)

add_executable(${PROJECT_NAME} ${SOURCE_DIR})
target_include_directories(chess PUBLIC "${PROJECT_BINARY_DIR}")
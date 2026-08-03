cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(EDRSMacroRenderer VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

create_geode_file(${PROJECT_NAME})

# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

 cmake_minimum_required(VERSION 3.10)

 project(michaelsoft-chip8)

 find_package(SDL2 REQUIRED)
 include_directories(${SDL2_INCLUDE_DIRS})

 set(SOURCE main.cpp chip8.hpp chip8.cpp)

 add_executable(${PROJECT_NAME} ${SOURCE})
 target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

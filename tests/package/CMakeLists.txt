cmake_minimum_required(VERSION 3.28)
project(dxfio_package LANGUAGES CXX)

find_package(dxfio CONFIG REQUIRED)

add_executable(dxfio_package main.cpp)

target_link_libraries(dxfio_package PRIVATE dxfio::dxfio)

cmake_minimum_required(VERSION 2.8.4)
project(settings)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (CMAKE_HOST_WIN32)
    add_definitions(-DMY_TO_STRING)
endif()

add_library(settings SHARED settings.cpp settings.hpp)
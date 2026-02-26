cmake_minimum_required(VERSION 3.10.0)
project(UnBBoolean VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB UnBBoolean_src CONFIGURE_DEPENDS "UnBBoolean/*.hpp" "UnBBoolean/*.cpp")

add_library(UnBBoolean ${UnBBoolean_src})

target_include_directories(UnBBoolean  PUBLIC ./)
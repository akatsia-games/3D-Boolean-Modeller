cmake_minimum_required(VERSION 3.10.0)
project(UnBBoolean VERSION 0.1.0 LANGUAGES C CXX)

add_library(UnBBoolean
    Point3f.hpp Point3f.cpp
    BooleanModeller.hpp BooleanModeller.cpp
    Bound.hpp Bound.cpp
    Face.cpp
    Line.cpp
    Object3D.cpp
    Segment.cpp
    Solid.hpp Solid.cpp
    Vertex.cpp)

target_include_directories(UnBBoolean  PUBLIC ./)
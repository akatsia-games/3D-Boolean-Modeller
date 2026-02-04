cmake_minimum_required(VERSION 3.10.0)
project(UnBBoolean VERSION 0.1.0 LANGUAGES C CXX)

add_library(UnBBoolean BooleanModeller.cpp Bound.cpp Face.cpp Line.cpp Object3D.cpp Segment.cpp Solid.cpp Vertex.cpp)


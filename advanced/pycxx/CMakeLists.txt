cmake_minimum_required(VERSION 3.2)
project(pycxx)

add_subdirectory(third_party/pybind11)
pybind11_add_module(pycxx foo.cc)


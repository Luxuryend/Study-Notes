cmake_minimum_required(VERSION 3.28)
project(example)

set(CMAKE_CXX_STANDARD 20)

add_executable(example main.cpp
        Student.cpp
        Student.h
        PrimaryStudent.cpp
        PrimaryStudent.h
        StudentManager.cpp
        StudentManager.h)

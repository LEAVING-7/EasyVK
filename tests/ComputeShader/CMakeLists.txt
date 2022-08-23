cmake_minimum_required(VERSION 3.10)
project(ComputeShader CXX)



add_executable(ComputeShader main.cpp)
target_link_libraries(ComputeShader PRIVATE EasyVK)


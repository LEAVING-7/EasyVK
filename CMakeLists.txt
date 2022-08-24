cmake_minimum_required(VERSION 3.10.0)
project(EasyVK)

option(EasyVK_TESTS "Build test" ON)

if (MSVC)
  add_compile_options(/utf-8)
endif()


find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
add_subdirectory(external)

add_library(EasyVK)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/include/EasyVK EasyVK_SRC)
target_include_directories(EasyVK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Vulkan_INCLUDE_DIR})
target_sources(EasyVK PUBLIC ${EasyVK_SRC} )
target_link_libraries(EasyVK  PUBLIC VulkanMemoryAllocator)

target_compile_features(EasyVK PUBLIC cxx_std_20)

if(EasyVK_TESTS) 
  add_subdirectory(tests)
endif()
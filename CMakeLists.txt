cmake_minimum_required(VERSION 3.10.0)
project(EasyVK)

if (MSVC)
  add_compile_options(/utf-8)
endif()

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
add_subdirectory(external)

add_library(EasyVK)

# file(GLOB_RECURSE CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/EasyVK/*.hpp)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/include/EasyVK EasyVK_SRC)
target_include_directories(EasyVK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Vulkan_INCLUDE_DIR})
target_sources(EasyVK PUBLIC ${EasyVK_SRC} )
target_link_libraries(EasyVK  PUBLIC VulkanMemoryAllocator)

target_compile_features(EasyVK PUBLIC cxx_std_20)
cmake_minimum_required(VERSION 3.10.0)
project(EasyVK)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})

add_library(EasyVK)

add_subdirectory(external)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src EasyVK_SRC)
target_include_directories(EasyVK INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${Vulkan_INCLUDE_DIR})
target_sources(EasyVK PUBLIC ${EasyVK_SRC})
target_link_libraries(EasyVK PUBLIC VulkanMemoryAllocator)

target_compile_features(EasyVK PUBLIC cxx_std_20)
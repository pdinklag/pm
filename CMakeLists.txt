cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)

# set build type to Release if nothing was specified
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" FORCE)
endif()

project(performance-measuring)

# set C++ build flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")

# create interface library (does not include malloc override)
add_library(pm INTERFACE)
target_include_directories(pm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# create library pm-malloc with malloc overrides
add_library(pm-malloc ${CMAKE_CURRENT_SOURCE_DIR}/src/malloc_callback.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/malloc_override.cpp)
target_compile_definitions(pm-malloc PUBLIC PM_MALLOC)
target_link_libraries(pm-malloc PUBLIC pm)

# provide tests if standalone
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()

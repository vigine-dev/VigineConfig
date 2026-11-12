cmake_minimum_required(VERSION 3.25)

project(VigineConfig CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VIGINECONFIG_BUILD_TESTS "Build VigineConfig tests (they come from VigineUnitTest)" OFF)
if(VIGINECONFIG_BUILD_TESTS)
    message(FATAL_ERROR "VigineConfig's tests come from VigineUnitTest")
endif()

# VigineFormat is found from CMAKE_PREFIX_PATH (<root>/install/<variant>).
find_package(VigineFormat REQUIRED)

file(GLOB VIGINECONFIG_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(vigineconfig STATIC ${VIGINECONFIG_SOURCES})
# The engine's kernel is a shared library that links this one in.
set_target_properties(vigineconfig PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(vigineconfig PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(vigineconfig PRIVATE VigineFormat::VigineFormat)

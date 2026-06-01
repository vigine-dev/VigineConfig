cmake_minimum_required(VERSION 3.25)

project(VigineConfig CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VIGINECONFIG_BUILD_TESTS "Build VigineConfig tests" ON)

# toml++ is header-only and vendored as a nested submodule.
set(TOMLPP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/external/tomlplusplus/include)

add_library(vigineconfig STATIC src/version.cpp)
target_include_directories(vigineconfig
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${TOMLPP_INCLUDE})

if(VIGINECONFIG_BUILD_TESTS)
    enable_testing()
    set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT for GoogleTest" FORCE)
    add_subdirectory(external/googletest EXCLUDE_FROM_ALL)

    add_executable(vigineconfig-smoke test/smoke_test.cpp)
    target_include_directories(vigineconfig-smoke PRIVATE ${TOMLPP_INCLUDE})
    target_link_libraries(vigineconfig-smoke PRIVATE vigineconfig gtest gtest_main)

    include(GoogleTest)
    gtest_discover_tests(vigineconfig-smoke)
endif()

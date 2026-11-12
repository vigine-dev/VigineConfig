cmake_minimum_required(VERSION 3.25)

project(VigineConfig CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VIGINECONFIG_BUILD_TESTS "Build VigineConfig tests (they come from VigineUnitTest)" ON)

# VigineFormat is found from CMAKE_PREFIX_PATH (<root>/install/<variant>).
find_package(VigineFormat REQUIRED)

set(VIGINECONFIG_SOURCES
    src/configkey.cpp
    src/configtree.cpp
    src/layeredstore.cpp
    src/releaseindex.cpp
    src/schemaregistry.cpp
    src/tomlcodec.cpp
    src/version.cpp)
list(TRANSFORM VIGINECONFIG_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

set(VIGINECONFIG_GENERATED_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/generated)
# VigineConfig is a shared library: it exports only its own interfaces, factories, value
# classes and free functions (marked VIGINECONFIG_API); everything else stays hidden inside.
add_library(vigineconfig SHARED ${VIGINECONFIG_SOURCES})

include(GenerateExportHeader)
generate_export_header(vigineconfig
    BASE_NAME VIGINECONFIG
    EXPORT_MACRO_NAME VIGINECONFIG_API
    EXPORT_FILE_NAME ${VIGINECONFIG_GENERATED_INCLUDE}/vigine/config/vigineconfig_export.h)

# Visibility, stated for every system rather than left to the defaults:
#  - hidden by default for C++ (a symbol is exported only when marked VIGINECONFIG_API);
#  - inline member functions hidden as well;
#  - on Windows nothing is exported unless dllexport-marked (no export-all);
#  - macOS: hidden visibility is the whole mechanism, the dylib exports what is marked and no more.
set_target_properties(vigineconfig PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
    POSITION_INDEPENDENT_CODE ON)
# A private STL member in an exported class is intended: the data is not part of the ABI contract.
if(MSVC)
    target_compile_options(vigineconfig PUBLIC /wd4251)
endif()

target_include_directories(vigineconfig
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<BUILD_INTERFACE:${VIGINECONFIG_GENERATED_INCLUDE}>
    PUBLIC $<INSTALL_INTERFACE:include>)
target_link_libraries(vigineconfig PRIVATE VigineFormat::VigineFormat)

# Linux: nothing from a linked library leaves through this one, and a version script exports only
# the vigine symbols, so the standard library's weak template instantiations stay private too.
if(UNIX AND NOT APPLE)
    target_link_options(vigineconfig PRIVATE
        "LINKER:--exclude-libs,ALL"
        "LINKER:--version-script=${CMAKE_CURRENT_SOURCE_DIR}/cmake/vigineconfig.map")
    set_property(TARGET vigineconfig APPEND PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vigineconfig.map)
endif()

# --- tests ---------------------------------------------------------------
# VigineUnitTest is found from CMAKE_PREFIX_PATH (<root>/install/<variant>).
if(VIGINECONFIG_BUILD_TESTS)
    enable_testing()
    find_package(VigineUnitTest REQUIRED)
    vigine_unit_add_tests(vigineconfig-test SOURCES
        test/configkey_test.cpp
        test/configtree_test.cpp
        test/configvalue_test.cpp
        test/layeredstore_test.cpp
        test/releaseindex_test.cpp
        test/schemaregistry_test.cpp
        test/smoke_test.cpp
        test/tomlcodec_test.cpp)
    target_link_libraries(vigineconfig-test PRIVATE vigineconfig)
endif()

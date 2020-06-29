#
# Copyright @ 2020 Audi AG. All rights reserved.
# 
#     This Source Code Form is subject to the terms of the Mozilla
#     Public License, v. 2.0. If a copy of the MPL was not distributed
#     with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
# 
# If it is not possible or desirable to put the notice in a particular file, then
# You may include the notice in a location (such as a LICENSE file in a
# relevant directory) where a recipient would be likely to look for such a notice.
# 
# You may add additional accurate notices of copyright ownership.

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0011 NEW)

# Disable extensions here and require the chosen CMAKE_CXX_STANDARD (coming from Conan)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use shared libraries from current directory on Linux (same behavior as on Windows)
SET(CMAKE_INSTALL_RPATH "$ORIGIN")

project(fep3-system-library VERSION 3.0.0)
set(FEP3_SYSTEM_LIBRARY fep3_system)

set(FEP3_SYSTEM_LIB_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(FEP3_SYSTEM_LIB_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(FEP3_SYSTEM_LIB_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(DOXYGEN_WARN_IF_UNDOCUMENTED "Yes")

set(FEP3_SYSTEM_LIB_VERSION "${FEP3_SYSTEM_LIB_VERSION_MAJOR}.${FEP3_SYSTEM_LIB_VERSION_MINOR}.${FEP3_SYSTEM_LIB_VERSION_PATCH}")

# Enable strict compiler warnings
if(MSVC)
    # TODO /WD4100 should be removed when ODAUTIL-167 is fixed
    # 4251 is currently deactivated because some dll exported classes use std types within their interface (e.g. ComponentRegistry)
    add_compile_options(/W4 /WX /wd4251 /wd4100)
else()
    # TODO -Wno-comment should be removed when ODAUTIL-169 is fixed
    add_compile_options(-Wall -Wno-unknown-pragmas -Wno-reorder -Werror -Wextra -pedantic -Wno-comment)
endif()

# check for conan
if(NOT CONAN_COMPILER)
    message(WARNING "Very good hint: FEP3 System Library should not be built without conan!")
endif()
if(CONAN_COMPILER)
    message(STATUS "Including Conan build info")
    if ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
    elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
    elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
        include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
    else()
        message(FATAL_ERROR "Conan build info can't be found.")
    endif()

    if(CORTEX_WORKSPACE)
        conan_basic_setup(TARGETS)
    else()
        conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
    endif()

    set(gtest_search_mode CONFIG)
    set(DOXYGEN_ROOT ${CONAN_DOXYGEN_ROOT})
endif()

option(fep3_system_cmake_enable_documentation
       "If enabled, generate the source code documentation -\
 requires doxygen and sphinx-build (default: ON)" ON)
option(fep3_system_cmake_enable_tests
       "Enable functional tests - requires googletest (default: OFF)" OFF)

include(scripts/cmake/enable_multicore_compilation.cmake)
include(scripts/cmake/use_integrated_debug_symbols.cmake)
include(src/fep_system/fep3_system-macros.cmake)

### Set basic and obligatory CMake parameters and find Project Templates package.
set(AEV_PRODUCT_FULL_NAME "FEP System Library - Functional Engineering Platform System Library")
set(AEV_PRODUCT_SHORT_NAME "fep-system-library")

# Enable project folder structure for Visual Studio IDE
set_property(GLOBAL PROPERTY USE_FOLDERS true)

### Product specific
set(FEP3_SYSTEM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BETA_BUILD false CACHE BOOL "Mark as beta")
set(REFERENCE_VERSION ${CONAN_PACKAGE_VERSION})

# some settings need to be set explicitly for QNX
if (UNIX)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
    set(CMAKE_SKIP_BUILD_RPATH  OFF)
endif(UNIX)

find_package(a_util REQUIRED)
find_package(pkg_rpc REQUIRED)
find_package(fep3_participant REQUIRED)
set(PARTICIPANT_LIB_DIR ${fep3_participant_DIR})

################################################################################
### Setting up packages
################################################################################

# compensate for the missing platform if building locally
if(NOT DEFINED PLATFORM)
    set(PLATFORM "developer")
endif(NOT DEFINED PLATFORM)

# set shared flags
set(FEP3_SYSTEM_SHARED_LIB ${BUILD_SHARED_LIBS})

configure_file(include/fep_system/fep_system_version.h.in include/fep_system/fep_system_version.h)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/include/fep_system/fep_system_version.h
    DESTINATION include/fep_system
)

# add subdirectories core
add_subdirectory(src)
if (fep3_system_cmake_enable_documentation)
    add_subdirectory(doc)
endif()
# install content from include directory
install(FILES README.md
        DESTINATION .)
install(FILES doc/changelog.md
        DESTINATION doc)

if (fep3_system_cmake_enable_tests)
    enable_testing()
    set(FEP3_TESTS_INTEGRATED true)
    add_subdirectory(test)
endif()

# install content from include directory
install(
    DIRECTORY
        include
    DESTINATION
        ./
    FILES_MATCHING PATTERN
        "*.h"
)

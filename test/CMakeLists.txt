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
# 

option(fep3_system_cmake_enable_functional_tests
       "Enable functional tests - requires googletest (default: OFF)" OFF)

if(NOT FEP3_TESTS_INTEGRATED)
    cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

    # Disable extensions here and require the chosen CMAKE_CXX_STANDARD (coming from e.g. Conan)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0011 NEW)

    project(fep-sdk-system-library-tests)

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
        set(fep3_system_DIR ${CONAN_FEP_SDK_SYSTEM_ROOT})
        set(fep3_participant_DIR ${CONAN_FEP_SDK_PARTICIPANT_ROOT})
    endif()

    # Enable project folder structure for Visual Studio IDE
    set_property(GLOBAL PROPERTY USE_FOLDERS true)

    include(../scripts/cmake/enable_multicore_compilation.cmake)
    include(../scripts/cmake/use_integrated_debug_symbols.cmake)

    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
    set (FEP_POST_BUILD_COPY true)

    find_package(fep3_system REQUIRED)
else()
    set(fep3_system_DIR ${CMAKE_INSTALL_PREFIX})
endif()

enable_testing()
find_package(fep3_participant_core REQUIRED CONFIG PATHS ${PARTICIPANT_LIB_DIR})

find_package(GTest REQUIRED ${gtest_search_mode})
find_package(GMock QUIET ${gtest_search_mode})

if(NOT GMock_FOUND) # backup
	get_target_property(GMOCK_LIB_DIRS GTest::GTest IMPORTED_LOCATION)
	get_filename_component(GMOCK_LIB_DIRS ${GMOCK_LIB_DIRS} DIRECTORY)
	# assuming gmock in the same location as gtest (hacky, I know)

    add_library(GMock::GMock STATIC IMPORTED)
	set_target_properties(GMock::GMock PROPERTIES
	  INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
	set_target_properties(GMock::GMock PROPERTIES
	  IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
	  IMPORTED_LOCATION "${GMOCK_LIB_DIRS}/gmock${CMAKE_STATIC_LIBRARY_SUFFIX}")

    add_library(GMock::Main STATIC IMPORTED)
	set_target_properties(GMock::Main PROPERTIES
	  INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
	set_target_properties(GMock::Main PROPERTIES
	  IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
	  IMPORTED_LOCATION "${GMOCK_LIB_DIRS}/gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

include(FEP_TEST_PRIVATE.cmake)

# for test_setup.h
include_directories(${CMAKE_CURRENT_LIST_DIR})

# tests have access to internal header
include_directories(../src)

set(CMAKE_BUILD_WITH_INSTALL_RPATH_BAK ${CMAKE_BUILD_WITH_INSTALL_RPATH})
if (UNIX AND FEP3_TESTS_INTEGRATED)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
endif()

if(fep3_system_cmake_enable_functional_tests)
    add_subdirectory(function)
endif()

if (UNIX AND FEP3_TESTS_INTEGRATED)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH_BAK})
endif()

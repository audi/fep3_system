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

# Using this to workaround MSCGEN bug in a_utils_add_doc
function(fep_add_doc CONFIG OUTPUT_DIR)
    find_package(Doxygen REQUIRED)

    if (NOT DOXYGEN_DOT_FOUND)
        message(FATAL_ERROR "Unable to find the dot tool, please set DOXYGEN_DOT_EXECUTABLE variable accordingly")
    endif (NOT DOXYGEN_DOT_FOUND)

    if(WIN32)
        set(MSCGEN_BIN mscgen.exe)
    else(WIN32)
        set(MSCGEN_BIN mscgen)
    endif(WIN32)

    find_path(MSCGEN_PATH ${MSCGEN_BIN} PATHS ${DOXYGEN_DIR}/bin)
    if(NOT MSCGEN_PATH)
        message(FATAL_ERROR "Unable to find the mscgen tool, please set MSCGEN_PATH variable accordingly")
    endif(NOT MSCGEN_PATH)

    set(REL_OUTPUT_DIR ${OUTPUT_DIR})
    set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_DIR})
    set(NEW_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg)
    configure_file(${CONFIG} ${NEW_CONFIG})
    
    ##Check optional parameters
    # May be overwritten in upcoming tests
    set(DOC_NAME DOC)
    
    ##Go through all available parameters
    set(LOOPCOUNT 0)                            #Needed to access variables
    foreach(f ${ARGV})
        math(EXPR LOOPCOUNT "${LOOPCOUNT}+1")   #Increases the loop count
        #Check for selected target name
        if(TARGET_NAME STREQUAL ${f})           #Another target name was set
            if(ARGV${LOOPCOUNT})                #Evaluates to ARGVx
                set(DOC_NAME ${ARGV${LOOPCOUNT}}) #Reset doc name
            else(ARGV${LOOPCOUNT})
                message(FATAL_ERROR "TARGET_NAME must not be empty!")
            endif(ARGV${LOOPCOUNT})
        endif(TARGET_NAME STREQUAL ${f})
    endforeach()
    
    # attach DOC target to ALL in release builds
    if(NOT ${CONAN_SETTINGS_BUILD_TYPE} STREQUAL "Debug")
        add_custom_target(${DOC_NAME} ALL ${DOXYGEN_EXECUTABLE} ${NEW_CONFIG}
            DEPENDS ${NEW_CONFIG}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    else()
        add_custom_target(${DOC_NAME} ${DOXYGEN_EXECUTABLE} ${NEW_CONFIG}
            DEPENDS ${NEW_CONFIG}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endfunction(fep_add_doc CONFIG OUTPUT)

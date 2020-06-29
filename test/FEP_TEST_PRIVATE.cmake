#######################################################################
#
# FEP SDK Test Config File
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
#
#######################################################################
macro(fep_add_gtest NAME TIMEOUT_S WORKING_DIRECTORY)
    add_executable(${NAME} ${ARGN})
    add_test(${NAME} ${NAME} WORKING_DIRECTORY ${WORKING_DIRECTORY})
    if (TEST ${NAME})
        set_tests_properties(${NAME} PROPERTIES TIMEOUT ${TIMEOUT_S})
    endif()
    target_link_libraries(${NAME} PRIVATE ${FEP_SDK_PARTICIPANT} GTest::Main)
    set_target_properties(${NAME} PROPERTIES INSTALL_RPATH "@CMAKE_CURRENT_BINARY_DIR@/src/")
    fep_deploy_libraries(${NAME})
endmacro()

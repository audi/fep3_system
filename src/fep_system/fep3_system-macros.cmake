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

macro(fep3_system_set_folder NAME FOLDER)
    set_property(TARGET ${NAME} PROPERTY FOLDER ${FOLDER})
endmacro(fep3_system_set_folder NAME FOLDER)

################################################################################
## \page page_cmake_commands
# <hr>
# <b>fep_install(\<name\> \<destination\>)</b>
#
# This macro installs the target \<name\>, together with the FEP SDK libraries (if neccessary)
#   to the folder \<destination\>
# Arguments:
# \li \<name\>:
# The name of the library to install.
# \li \<destination\>:
# The relative path to the install subdirectory
################################################################################
macro(fep3_system_install NAME DESTINATION)
    install(TARGETS ${NAME} DESTINATION ${DESTINATION})
    
    install(FILES $<TARGET_FILE:fep3_system> DESTINATION ${DESTINATION})
    install(FILES $<TARGET_FILE_DIR:fep3_system>/fep3_system.plugins DESTINATION ${DESTINATION})
    install(FILES $<TARGET_FILE:fep3_http_service_bus> DESTINATION ${DESTINATION}/http)

endmacro(fep3_system_install NAME DESTINATION)

macro(fep3_system_deploy NAME)
    # no need to copy in build directory on linux since linker rpath takes care of that
    if (WIN32)
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:fep3_system>" "$<TARGET_FILE_DIR:${NAME}>"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${NAME}>/http"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:fep3_http_service_bus>" "$<TARGET_FILE_DIR:${NAME}>/http"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE_DIR:fep3_system>/fep3_system.plugins" "$<TARGET_FILE_DIR:${NAME}>"
        )
    endif()

    set_target_properties(${NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
endmacro(fep3_system_deploy NAME)


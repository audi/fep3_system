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

# fep system library 
set(SYSTEM_SOURCES_PUBLIC
    ${PROJECT_SOURCE_DIR}/include/fep_system/fep_system_export.h
    ${PROJECT_SOURCE_DIR}/include/fep_system/fep_system_types.h
    ${PROJECT_SOURCE_DIR}/include/fep_system/fep_system.h
    ${PROJECT_SOURCE_DIR}/include/fep_system/system_logger_intf.h
    ${PROJECT_SOURCE_DIR}/include/fep_system/participant_proxy.h
    ${PROJECT_SOURCE_DIR}/include/fep_system/rpc_component_proxy.h)

# install destination should not be forgotten: include/fep_system/rpc_services/rpc
set(SYSTEM_EXT_PUBLIC_SOURCES_RPC
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/base/fep_rpc_iid.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/base/fep_rpc_client_intf.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/base/fep_rpc_client.h
    )
    
# install destination should not be forgotten: include/fep_system/base/logging
set(SYSTEM_EXT_PUBLIC_SOURCES_LOGGING_BASE
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/logging/logging_types.h
    )

# subtle difference: on unix the command silently fails if the output directory does not exist...
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs) 

# subtle difference: on unix the command silently fails if the output directory does not exist...
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include/fep_system/rpc_services) 

###################################################
# Participant Info
###################################################

# install destination should not be forgotten: include/fep_system/rpc_services/participant_info
set(SYSTEM_EXT_PUBLIC_SOURCES_PARTICIPANT_INFO
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/participant_info/participant_info_rpc_intf_def.h
    )
     
jsonrpc_generate_client_stub(${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/participant_info/participant_info.json
                             fep3::rpc_proxy_stub::RPCParticipantInfoProxy
                             ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/participant_info_proxy_stub.h)
     

###################################################
# FEP 3 State Machine
###################################################

# install destination should not be forgotten: include/fep_system/rpc_services/participant_statemachine
set(SYSTEM_EXT_PUBLIC_SOURCES_STATE_MACHINE_FEP
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/participant_statemachine/participant_statemachine_rpc_intf_def.h
    )
    
jsonrpc_generate_client_stub(${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/participant_statemachine/participant_statemachine.json
                             fep3::rpc_proxy_stub::RPCStateMachineProxy
                             ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/participant_statemachine_proxy_stub.h)

###################################################
# FEP 3 Clock
###################################################

# install destination should not be forgotten: include/fep_system/rpc_services/clock
set(SYSTEM_EXT_PUBLIC_SOURCES_CLOCK_FEP
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/clock/clock_service_rpc_intf_def.h
    )
    
jsonrpc_generate_client_stub(${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/clock/clock.json
                             fep3::rpc_proxy_stub::RPCClockServiceProxy
                             ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/clock_proxy_stub.h)
 

###################################################
# Data Registry
###################################################

# install destination should not be forgotten: include/fep_system/rpc_services/data_registry
set(SYSTEM_EXT_PUBLIC_SOURCES_DATA_REGISTRY
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/data_registry/data_registry_rpc_intf_def.h
    )

# install destination should not be forgotten: ${PROJECT_BINARY_DIR}/include/fep_system/rpc_services/data_registry
set(SYSTEM_EXT_PUBLIC_SOURCES_STREAMTYPE_BASE
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/streamtype/default_streamtype.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/streamtype/streamtype.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/streamtype/streamtype_intf.h
    ) 
    
# properties are needed for loggin, dataregistry (streamtype) and configuration service
# install destination should not be forgotten: ${PROJECT_BINARY_DIR}/include/fep_system/rpc_services/properties
set(SYSTEM_EXT_PUBLIC_SOURCES_PROPERTY_BASE
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/properties/properties.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/properties/properties_intf.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/properties/property_type.h
    ${PARTICIPANT_LIB_DIR}/include/fep3/base/properties/property_type_conversion.h
    )

jsonrpc_generate_client_stub(
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/data_registry/data_registry.json
    fep3::rpc_proxy_stub::RPCDataRegistryProxy
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/data_registry_proxy_stub.h)

# properties for the moment
file(COPY ${SYSTEM_EXT_PUBLIC_SOURCES_PROPERTY_BASE}
     DESTINATION ${PROJECT_BINARY_DIR}/include/fep_system/base/properties)


###################################################
# Logging Service
###################################################

# install destination should not be forgotten: include/fep_system/rpc_services/logging
set(SYSTEM_EXT_PUBLIC_SOURCES_LOGGING
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/logging/logging_service_rpc_intf_def.h
    )

jsonrpc_generate_client_stub(
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/logging/logging_service.json
    fep3::rpc_proxy_stub::RPCLoggingServiceProxy
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/logging_service_proxy_stub.h)

jsonrpc_generate_client_stub(
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/logging/logging_rpc_sink_service.json
    fep3::rpc_proxy_stub::RPCLoggingSinkService
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/logging_sink_service_stub.h)

jsonrpc_generate_server_stub(
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/logging/logging_rpc_sink_client.json
    fep3::rpc_proxy_stub::RPCLoggingSink
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/logging_sink_stub.h)


###################################################
# Configuration
################################################### 

# install destination should not be forgotten: include/fep_system/rpc_services/configuration
set(SYSTEM_EXT_PUBLIC_SOURCES_CONFIGURATION
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/configuration/configuration_rpc_intf_def.h
    )

jsonrpc_generate_client_stub(
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/configuration/configuration.json
    fep3::rpc_proxy_stub::RPCConfigurationServiceProxy
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/configuration_service_proxy_stub.h)

###################################################
# collect all private proxy implementations
################################################### 

set(SYSTEM_PROXY_SOURCES_PRIVATE
    rpc_services/participant_info_proxy.hpp
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/participant_info_proxy_stub.h

    rpc_services/participant_statemachine_proxy.hpp
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/participant_statemachine_proxy_stub.h

    rpc_services/clock_proxy.hpp
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/clock_proxy_stub.h

    rpc_services/data_registry_proxy.hpp
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/data_registry_proxy_stub.h
    
    rpc_services/logging_proxy.hpp
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/logging_service_proxy_stub.h
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/logging_sink_service_stub.h

    rpc_services/configuration_proxy.hpp
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/configuration_service_proxy_stub.h
)

### plugin things
set(PLUGIN_BASE_DIR ${PROJECT_SOURCE_DIR}/src/fep3/plugin/base)
set(PLUGIN_CPP_DIR ${PROJECT_SOURCE_DIR}/src/fep3/plugin/cpp)
set(PLUGIN_CPP_SOURCES_PRIVATE
    ${PLUGIN_CPP_DIR}/cpp_host_plugin.cpp
    ${PLUGIN_CPP_DIR}/cpp_host_plugin.h
)

set(PLUGIN_SOURCES_PRIVATE
    ${PLUGIN_BASE_DIR}/shared_library.h
    ${PLUGIN_BASE_DIR}/shared_library.cpp
    ${PLUGIN_BASE_DIR}/host_plugin_base.h
    ${PLUGIN_BASE_DIR}/host_plugin_base.cpp
)

set(PLUGIN_SOURCES ${PLUGIN_CPP_SOURCES_PRIVATE} ${PLUGIN_SOURCES_PRIVATE})
source_group(plugin\\cpp FILES ${PLUGIN_SOURCES})

### evaentually some really target files
    
set(SYSTEM_SOURCES_PRIVATE
    fep_system.cpp
    participant_proxy.cpp
    system_logger.h
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs/logging_sink_stub.h
    service_bus_factory.h
    service_bus_factory.cpp
    private_participant_proxy.hpp)

add_library(${FEP3_SYSTEM_LIBRARY} SHARED
    ${SYSTEM_SOURCES_PUBLIC}
    ${SYSTEM_SOURCES_PRIVATE}
    ${SYSTEM_PROXY_SOURCES_PRIVATE}
    ${SYSTEM_EXT_PUBLIC_SOURCES_RPC}
    ${SYSTEM_EXT_PUBLIC_SOURCES_LOGGING_BASE}
    
    ${PLUGIN_SOURCES}

    ${SYSTEM_EXT_PUBLIC_SOURCES_PARTICIPANT_INFO}

    ${SYSTEM_EXT_PUBLIC_SOURCES_STATE_MACHINE_FEP}

    ${SYSTEM_EXT_PUBLIC_SOURCES_CLOCK_FEP}
    
    ${SYSTEM_EXT_PUBLIC_SOURCES_LOGGING}
    
    ${SYSTEM_EXT_PUBLIC_SOURCES_CONFIGURATION}
    ${SYSTEM_EXT_PUBLIC_SOURCES_PROPERTY_BASE}

    ${SYSTEM_EXT_PUBLIC_SOURCES_DATA_REGISTRY}
    ${SYSTEM_EXT_PUBLIC_SOURCES_STREAMTYPE_BASE}
)

set_target_properties(${FEP3_SYSTEM_LIBRARY} PROPERTIES
    OUTPUT_NAME ${FEP3_SYSTEM_LIBRARY}
    DEBUG_POSTFIX "d${FEP3_SYSTEM_LIB_VERSION_MAJOR}.${FEP3_SYSTEM_LIB_VERSION_MINOR}"
    RELEASE_POSTFIX "${FEP3_SYSTEM_LIB_VERSION_MAJOR}.${FEP3_SYSTEM_LIB_VERSION_MINOR}"
    RELWITHDEBINFO_POSTFIX "${FEP3_SYSTEM_LIB_VERSION_MAJOR}.${FEP3_SYSTEM_LIB_VERSION_MINOR}"
)

set_target_properties(${FEP3_SYSTEM_LIBRARY} PROPERTIES CXX_VISIBILITY_PRESET hidden)

set(BUILD_INTERFACE_INCLUDES 
    ${CMAKE_CURRENT_LIST_DIR} 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/fep_system
    ${PROJECT_SOURCE_DIR}/src
    ${PARTICIPANT_LIB_DIR}/include
    ${PARTICIPANT_LIB_DIR}/include/fep3
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/participant_info
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/participant_statemachine
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/clock
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/data_registry
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/logging
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/configuration
    ${PARTICIPANT_LIB_DIR}/include/fep3/rpc_services/base
    ${PROJECT_BINARY_DIR}/src/fep_system
    ${PROJECT_BINARY_DIR}/src/fep_system/fep_system_stubs
)

set(INSTALL_INTERFACE_INCLUDES 
    include)

if(WIN32)
    set(BUILD_LIBRARY_FLAGS "")
else()
    set(BUILD_LIBRARY_FLAGS "-Wl,--start-group;-Wl,--no-as-needed")
endif()

target_link_libraries(${FEP3_SYSTEM_LIBRARY}
    PRIVATE
        a_util_datetime
        a_util_system
        a_util_process
        a_util_strings
        a_util_xml
        pkg_rpc
    PUBLIC
        ${BUILD_LIBRARY_FLAGS}
    
)

target_include_directories(${FEP3_SYSTEM_LIBRARY} PUBLIC
    "$<BUILD_INTERFACE:${BUILD_INTERFACE_INCLUDES}>"
    "$<INSTALL_INTERFACE:${INSTALL_INTERFACE_INCLUDES}>"
)
#install base types needed within the interfaces
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_RPC} DESTINATION include/fep_system/rpc_services/base)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_LOGGING_BASE} DESTINATION include/fep_system/base/logging)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_PROPERTY_BASE} DESTINATION include/fep_system/base/properties)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_STREAMTYPE_BASE} DESTINATION include/fep_system/base/streamtype)

install(DIRECTORY ${PROJECT_BINARY_DIR}/include/fep_system/rpc_services DESTINATION include/fep_system)

install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_PARTICIPANT_INFO} DESTINATION include/fep_system/rpc_services/participant_info)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_STATE_MACHINE_FEP} DESTINATION include/fep_system/rpc_services/participant_statemachine)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_CLOCK_FEP} DESTINATION include/fep_system/rpc_services/clock)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_DATA_REGISTRY} DESTINATION include/fep_system/rpc_services/data_registry)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_LOGGING} DESTINATION include/fep_system/rpc_services/logging)
install(FILES ${SYSTEM_EXT_PUBLIC_SOURCES_CONFIGURATION} DESTINATION include/fep_system/rpc_services/configuration)

target_compile_definitions(${FEP3_SYSTEM_LIBRARY} PRIVATE FEP3_SYSTEM_LIB_DO_EXPORT)
set_target_properties(${FEP3_SYSTEM_LIBRARY} PROPERTIES CXX_VISIBILITY_PRESET hidden)

fep3_system_set_folder(${FEP3_SYSTEM_LIBRARY} system_library)
source_group(system_library\\public FILES ${SYSTEM_SOURCES_PUBLIC})
source_group(system_library FILES ${SYSTEM_SOURCES_PRIVATE})
source_group(system_library\\proxy FILES ${SYSTEM_PROXY_SOURCES_PRIVATE})

#this is for configre in the fep3_system-config.cmake.in
set(FEP3_USE_RTIDDS ${fep3_participant_use_rtidds})
#this is for using in install and deploy
set(fep3_system_use_rtidds ${FEP3_USE_RTIDDS})

configure_file(fep3_system-config.cmake.in ${CMAKE_BINARY_DIR}/fep3_system-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/fep3_system-config.cmake DESTINATION .)


if(WIN32)
    set(TARGET_SERVICE_BUS ./http/fep3_http_service_bus.dll)
else()
    set(TARGET_SERVICE_BUS ./http/libfep3_http_service_bus.so)
endif()

configure_file(fep3_system.plugins.in ${CMAKE_BINARY_DIR}/fep3_system.plugins @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/fep3_system.plugins DESTINATION lib)
add_custom_command(TARGET ${FEP3_SYSTEM_LIBRARY} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/fep3_system.plugins" "$<TARGET_FILE_DIR:${FEP3_SYSTEM_LIBRARY}>"
    )


# Create version config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/fep3_system-config-version.cmake
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fep3_system-config-version.cmake DESTINATION .)
install(FILES fep3_system-macros.cmake DESTINATION .)

install(TARGETS ${FEP3_SYSTEM_LIBRARY} 
    EXPORT fep3_system_export
    DESTINATION lib
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT fep3_system_export DESTINATION lib/cmake
    FILE fep3_system_targets.cmake
)
#this is to copy all relevant dependencies into the right folders
fep3_system_install(${FEP3_SYSTEM_LIBRARY} lib)
set(fep3_system_no_copy_pluginfile True)
fep3_system_deploy(${FEP3_SYSTEM_LIBRARY})

#but we need to install the targets.cmake files from participant to use it at cmake file of the fep3_system
install(DIRECTORY ${PARTICIPANT_LIB_DIR}/lib/cmake
    DESTINATION lib
    FILES_MATCHING PATTERN "*service_bus*.cmake")

if(WIN32)
    install(FILES $<TARGET_PDB_FILE:fep3_system> DESTINATION lib OPTIONAL)
endif(WIN32)

#install service bus plugin to build folder, to be used within integrated tests 
#(it will be referenced relative to the system library in the build folder)
if(UNIX)
    install(FILES $<TARGET_FILE:fep3_http_service_bus> DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/http)
endif()


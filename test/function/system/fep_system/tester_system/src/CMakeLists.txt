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


##################################################################
# tester_system_discovery
##################################################################

set(_current_test_name tester_system_discovery)
add_executable(${_current_test_name} system_discovery.cpp fep_test_common.h)
target_link_libraries(${_current_test_name} 
	              PRIVATE GTest::Main fep3_system fep3_participant_core a_util_process)
set_target_PROPERTIES(${_current_test_name} PROPERTIES FOLDER test/fep_system)
add_test(NAME ${_current_test_name} 
	 COMMAND ${_current_test_name}
	 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
fep3_system_deploy(${_current_test_name})
#we need also the participant in our test to create the test participants
fep3_participant_deploy(${_current_test_name})

##################################################################
# tester_system
##################################################################

set(_current_test_name tester_system)
add_executable(${_current_test_name} system.cpp fep_test_common.h)
target_link_libraries(${_current_test_name} 
	              PRIVATE GTest::Main fep3_system fep3_participant_core a_util_process)
set_target_PROPERTIES(${_current_test_name} PROPERTIES FOLDER test/fep_system)
add_test(NAME ${_current_test_name} 
	 COMMAND ${_current_test_name}
	 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
fep3_system_deploy(${_current_test_name})
#we need also the participant in our test to create the test participants
fep3_participant_deploy(${_current_test_name})

##################################################################
# tester_system_library_participant
##################################################################

set(_current_test_name tester_system_library_participant)
add_executable(${_current_test_name} participant.cpp fep_test_common.h)
target_link_libraries(${_current_test_name} 
	              PRIVATE GTest::Main fep3_system fep3_participant_core a_util_process)
set_target_PROPERTIES(${_current_test_name} PROPERTIES FOLDER test/fep_system)
add_test(NAME ${_current_test_name} 
	 COMMAND ${_current_test_name}
	 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
fep3_system_deploy(${_current_test_name})
#we need also the participant in our test to create the test participants
fep3_participant_deploy(${_current_test_name})

##################################################################
# tester_system_library_part_config
##################################################################

set(_current_test_name tester_system_library_part_config)
add_executable(${_current_test_name} participant_configuration.cpp fep_test_common.h)
target_link_libraries(${_current_test_name} 
	PRIVATE GMock::GMock GTest::Main fep3_system fep3_participant_core a_util_process)
set_target_PROPERTIES(${_current_test_name} PROPERTIES FOLDER test/fep_system)
add_test(NAME ${_current_test_name} 
	 COMMAND ${_current_test_name}
	 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
fep3_system_deploy(${_current_test_name})
#we need also the participant in our test to create the test participants
fep3_participant_deploy(${_current_test_name})

##################################################################
# tester_system_library_logging
##################################################################

set(_current_test_name tester_system_library_logging)
add_executable(${_current_test_name} system_logging.cpp fep_test_common.h)
target_link_libraries(${_current_test_name} 
	              PRIVATE GTest::Main fep3_system fep3_participant_core a_util_process)
set_target_PROPERTIES(${_current_test_name} PROPERTIES FOLDER test/fep_system)
add_test(NAME ${_current_test_name} 
	 COMMAND ${_current_test_name}
	 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../)
fep3_system_deploy(${_current_test_name})
#we need also the participant in our test to create the test participants
fep3_participant_deploy(${_current_test_name})

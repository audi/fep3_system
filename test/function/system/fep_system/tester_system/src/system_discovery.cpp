/**
 * @file

   @copyright
   @verbatim
   Copyright @ 2020 Audi AG. All rights reserved.
   
       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
   
   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.
   
   You may add additional accurate notices of copyright ownership.
   @endverbatim 
 *
 */
 
#include <gtest/gtest.h>
#include <fep_system/fep_system.h>
#include <string.h>
#include "fep_test_common.h"
#include "a_util/logging.h"
#include "a_util/process.h"

/**
 * @req_id FEPSDK-2128
 */
/*
TEST(SystemDiscovery, StandaloneModulesWillNotBeDiscovered)
{    
    const auto participant_names = std::vector<std::string>
        { "standalone_participant"
        , "participant_no_standalone_path"
        , "participant3" };

    const Modules modules = createTestModules(participant_names);

    ASSERT_EQ(
        modules.at("standalone_participant")->GetPropertyTree()->SetPropertyValue(FEP_STM_STANDALONE_PATH, true),
        a_util::result::Result());

    ASSERT_EQ(
        modules.at("participant_no_standalone_path")->GetPropertyTree()->DeleteProperty(FEP_STM_STANDALONE_PATH),
        a_util::result::Result());

    /// discover
    const auto domain_id = modules.begin()->second->GetDomainId();
    fep::System my_system = fep::discoverSystemOnDDS("my_system", domain_id, 1000);
    const auto discovered_participants = my_system.getParticipants();

    /// compare with expectation
    const std::vector<std::string> discoverd_names = [discovered_participants]() {
        std::vector<std::string> temp;
        std::transform
        (discovered_participants.begin()
            , discovered_participants.end()
            , std::back_inserter(temp)
            , [](decltype(discovered_participants)::value_type participant)
        {
            return participant.getName();
        }
        );
        return temp;
    }();    
    EXPECT_EQ(discoverd_names, std::vector<std::string>({"participant3", "participant_no_standalone_path"}));    
}*/

/**
 * @brief The discovery of participants is tested
 * @req_id 
 */
TEST(SystemDiscovery, DiscoverSystem)
{
    auto sys_name = makePlatformDepName("test_system");
    const auto participant_names = std::vector<std::string>{ "participant1", "participant2" };
    const TestParticipants test_parts = createTestParticipants(participant_names, sys_name);
    
    /// discover system
    fep3::System my_system;
    ASSERT_NO_THROW(
        my_system = std::move(fep3::discoverSystem(sys_name));
    );
    const auto discovered_participants = my_system.getParticipants();

    /// compare with expectation
    const std::vector<std::string> discovered_names = [discovered_participants](){
        std::vector<std::string> temp;
        std::transform
            (discovered_participants.begin()
            , discovered_participants.end()
            , std::back_inserter(temp)
            , [](decltype(discovered_participants)::value_type participant)
                {
                    return participant.getName();
                }    
            );
        return temp;
    }();
    EXPECT_EQ(discovered_names, participant_names);
}

/**
 * @brief The discovery of participants is tested
 * @req_id
 */
TEST(SystemDiscovery, DiscoverSystemAll)
{
    auto sys_name_1 = makePlatformDepName("test_system_number_1");
    const auto participant_names_1 = std::vector<std::string>{ "participant1", "participant2" };
    const TestParticipants test_parts_1 = createTestParticipants(participant_names_1, sys_name_1);

    auto sys_name_2 = makePlatformDepName("test_system_number_2");
    const auto participant_names_2 = std::vector<std::string>{ "participant1", "participant2" };
    const TestParticipants test_parts_2 = createTestParticipants(participant_names_2, sys_name_2);

    auto sys_name_3 = makePlatformDepName("test_system_number_3");
    const auto participant_names_3 = std::vector<std::string>{ "part_x", "part_y", "part_z"};
    const TestParticipants test_parts_3 = createTestParticipants(participant_names_3, sys_name_3);

    /// discover system
    std::vector<fep3::System> my_systems;
    ASSERT_NO_THROW(
        my_systems = std::move(fep3::discoverAllSystems());
    );

    //we can not assure that absolutely no other system is disovered here
    //all defaults using the same discovery url!!
    ASSERT_TRUE(my_systems.size() >= 3);

    bool system_1_found = false;
    bool system_2_found = false;
    bool system_3_found = false;

    for (auto const& current_system : my_systems)
    {
        const auto discovered_participants = current_system.getParticipants();
        decltype(participant_names_1)* current_names = nullptr;
        if (current_system.getSystemName() == sys_name_1)
        {
            system_1_found = true;
            current_names = &participant_names_1;
        }
        else if (current_system.getSystemName() == sys_name_2)
        {
            system_2_found = true;
            current_names = &participant_names_2;
        }
        else if (current_system.getSystemName() == sys_name_3)
        {
            system_3_found = true;
            current_names = &participant_names_3;
        }
        if (current_names)
        {
            /// compare with expectation
            const std::vector<std::string> discovered_names = [discovered_participants]() {
                std::vector<std::string> temp;
                std::transform
                (discovered_participants.begin()
                    , discovered_participants.end()
                    , std::back_inserter(temp)
                    , [](decltype(discovered_participants)::value_type participant)
                {
                    return participant.getName();
                }
                );
                return temp;
            }();
            EXPECT_EQ(discovered_names, *current_names);
        }
    }

    ASSERT_TRUE(system_1_found);
    ASSERT_TRUE(system_2_found);
    ASSERT_TRUE(system_3_found);
}

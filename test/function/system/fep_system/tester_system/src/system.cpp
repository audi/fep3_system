/**
 * Implementation of the tester for the FEP Data Sample (locking)
 *
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

 /**
 * Test Case:   TestSystemLibrary
 * Test ID:     1.0
 * Test Title:  FEP System Library base test
 * Description: Test if controlling a test system is possible
 * Strategy:    Invoke Controller and issue commands
 * Passed If:   no errors occur
 * Ticket:      -
 * Requirement: -
 */

#include <gtest/gtest.h>
#include <fep_system/fep_system.h>
#include <string.h>
#include "fep_test_common.h"
#include <a_util/logging.h>
#include <a_util/process.h>

#include <fep3/components/configuration/configuration_service_intf.h>
#include <fep3/components/clock/clock_service_intf.h>
#include <fep3/components/clock_sync/clock_sync_service_intf.h>
#include <fep3/components/scheduler/scheduler_service_intf.h>
#include <fep3/components/configuration/propertynode_helper.h>

void addingTestParticipants(fep3::System& sys)
{
    sys.add("part1");
    sys.add("part2");
    auto part1 = sys.getParticipant("part1");
    part1.setInitPriority(1);
    part1.setStartPriority(11);
    part1.setAdditionalInfo("part1", "part1_value");
    part1.setAdditionalInfo("part1_2", "part1_value_2");

    auto part2 = sys.getParticipant("part2");
    part2.setInitPriority(2);
    part2.setStartPriority(22);
    part2.setAdditionalInfo("part2", "part2_value");
}

bool hasTestParticipants(fep3::System& sys)
{
    try
    {
        auto part1 = sys.getParticipant("part1");
        if (!(part1.getInitPriority() == 1))
        {
            return false;
        }
        if (!(part1.getStartPriority() == 11))
        {
            return false;
        }
        if (!(part1.getAdditionalInfo("part1", "") == "part1_value"))
        {
            return false;
        }
        if (!(part1.getAdditionalInfo("part1_2", "") == "part1_value_2"))
        {
            return false;
        }

        auto part2 = sys.getParticipant("part2");
        if (!(part2.getInitPriority() == 2))
        {
            return false;
        }
        if (!(part2.getStartPriority() == 22))
        {
            return false;
        }
        if (!(part2.getAdditionalInfo("part2", "") == "part2_value"))
        {
            return false;
        }
        return true;
    }
    catch (const std::runtime_error&)
    {
        return false;
    }
}

 /**
 * @brief Test teh CTORs of the fep::system
 * @req_id
 */

TEST(SystemLibrary, SystemCtors)
{
    using namespace fep3;
    {
        //default CTOR will have a system_name which is empty
        System test_system_default;
        addingTestParticipants(test_system_default);
        ASSERT_EQ(test_system_default.getSystemName(), "");
        ASSERT_TRUE(hasTestParticipants(test_system_default));
    }

    {
        //spec CTOR will have a system_name which is empty
        System test_system("system_name");
        ASSERT_EQ(test_system.getSystemName(), "system_name");
        ASSERT_FALSE(hasTestParticipants(test_system));
    }

    {
        //move CTOR will have a system_name which is empty
        System test_system("system_name");
        addingTestParticipants(test_system);
        System moved_sys(std::move(test_system));
        ASSERT_EQ(moved_sys.getSystemName(), "system_name");
        ASSERT_TRUE(hasTestParticipants(moved_sys));
    }

    {
        System test_system("system_name");
        addingTestParticipants(test_system);
        //copy CTOR will have a system_name which is empty
        System copied_sys(test_system);
        ASSERT_EQ(copied_sys.getSystemName(), "system_name");
        ASSERT_EQ(test_system.getSystemName(), "system_name");
        ASSERT_TRUE(hasTestParticipants(test_system));
        ASSERT_TRUE(hasTestParticipants(copied_sys));
    }

    {
        System test_system("orig");
        addingTestParticipants(test_system);
        //copy CTOR will have a system_name which is empty
        System copied_assigned_sys = test_system;
        ASSERT_EQ(test_system.getSystemName(), "orig");
        ASSERT_EQ(copied_assigned_sys.getSystemName(), "orig");
        ASSERT_TRUE(hasTestParticipants(test_system));
        ASSERT_TRUE(hasTestParticipants(copied_assigned_sys));
    }

    {
        //moved CTOR will have a system_name which is empty
        System test_system("orig");
        addingTestParticipants(test_system);
        System moved_assigned_sys = std::move(test_system);
        ASSERT_EQ(moved_assigned_sys.getSystemName(), "orig");
        ASSERT_TRUE(hasTestParticipants(moved_assigned_sys));
    }
}

class TestEventMonitor : public fep3::IEventMonitor
{
public:
    TestEventMonitor(): _logger_name_filter("")
    {
    }
    TestEventMonitor(const std::string& logger_name_filter) : _logger_name_filter(logger_name_filter)
    {

    }
    void onLog(std::chrono::milliseconds,
        fep3::logging::Severity severity_level,
        const std::string& participant_name,
        const std::string& logger_name, //depends on the Category ... 
        const std::string& message) override
    {
        if (!_logger_name_filter.empty())
        {
            if (_logger_name_filter != logger_name)
            {
                return;
            }
        }
        std::unique_lock<std::mutex> lk(_cv_m);
        _severity_level = severity_level;
        _participant_name = participant_name;
        _logger_name = logger_name;
        _message = message;
        _done = true;
    }

    bool waitForDone(timestamp_t timeout_ms = 4000)
    {
        if (_done)
        {
            _done = false;
            return true;
        }
        int i = 0;
        auto single_wait = timeout_ms / 10;
        while (!_done && i < 10)
        {
            a_util::system::sleepMilliseconds(static_cast<uint32_t>(single_wait));
            ++i;
        }
        if (_done)
        {
            _done = false;
            return true;
        }
        return false;
    }

    bool waitFor(const std::string& message, timestamp_t timeout_ms = 4000)
    {
        auto begin_time = a_util::system::getCurrentMilliseconds();
        {
            auto single_wait = timeout_ms / 10;
            while (timeout_ms > (a_util::system::getCurrentMilliseconds() - begin_time))
            {
                std::string current_message;
                {
                    std::unique_lock<std::mutex> lk(_cv_m);
                    current_message = _message;
                }
                if (current_message.find(message) != std::string::npos)
                {
                    return true;
                }
                a_util::system::sleepMilliseconds(static_cast<uint32_t>(single_wait));
            }
        }
        std::string current_message_final;
        {
            std::unique_lock<std::mutex> lk(_cv_m);
            current_message_final = _message;
        }
        return (current_message_final.find(message) != std::string::npos);
    }

    fep3::logging::Category _category;
    fep3::logging::Severity _severity_level;
    std::string _participant_name;
    std::string _message;
    std::string _logger_name;
    std::vector<fep3::rpc::ParticipantState> _states;
    std::string _new_name;
    std::string _logger_name_filter;
private:
    std::mutex _cv_m;
    std::atomic_bool _done{ false };
};

/**
* @brief It's tested that a system containing standalone participants can not be started
* @req_id FEPSDK-2129
*/
/* readd ... when paticiapnts without a statemachine can be added
AEV_TEST(SystemLibrary, SystemWithStandaloneParticipantCanNotBeStarted, "")
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const auto participant_names = std::vector<std::string>{ "participant1", "participant2" };
    const auto test_parts = createTestParticipants(participant_names, sys_name);
        
    ASSERT_EQ(
        modules.begin()->second->GetPropertyTree()->SetPropertyValue(FEP_STM_STANDALONE_PATH, true),
        a_util::result::Result());

    fep::System my_system = fep::System("my_system");
    EXPECT_NO_THROW(my_system.add(participant_names));

    /// start fails because standalone module is part of fep system
    EXPECT_THROW(my_system.start(), std::runtime_error);
}*/

TEST(SystemLibrary, TestConfigureSystemOK)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const std::string part_name = "participant1";
    
    const auto participant_names = std::vector<std::string>{ part_name };
    const auto test_parts = createTestParticipants(participant_names, sys_name);

    {
        fep3::System my_sys(sys_name);
        my_sys.add(part_name);
        my_sys.add("Participant2");
        my_sys.remove("Participant2");
        my_sys.getParticipant(part_name).setInitPriority(1);
        auto p1 = my_sys.getParticipant(part_name);
        ASSERT_EQ(part_name, p1.getName());
        my_sys.clear();

        auto ps = my_sys.getParticipants();
        ASSERT_TRUE(ps.empty());
        ASSERT_EQ(my_sys.getSystemName(), sys_name);
    }

}

TEST(SystemLibrary, TestConfigureSystemNOK)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    {
        fep3::System my_sys(sys_name);

        bool caught = false;
        try
        {
            my_sys.getParticipant("does_not_exist");
        }
        catch (std::runtime_error e)
        {
            auto msg = e.what();
            ASSERT_TRUE(a_util::strings::isEqual(msg, "No Participant with the name does_not_exist found"));
            caught = true;
        }
        ASSERT_TRUE(caught);

        ASSERT_TRUE(fep3::rpc::arya::IRPCParticipantStateMachine::State::undefined == my_sys.getSystemState()._state);
      
    }
}

TEST(SystemLibrary, TestControlSystemOK)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const std::string part_name_1 = "participant1";
    const std::string part_name_2 = "participant2";

    const auto participant_names = std::vector<std::string>{ part_name_1, part_name_2 };
    const auto test_parts = createTestParticipants(participant_names, sys_name);

    {
        fep3::System my_sys(sys_name);
        my_sys.add(part_name_1);
        my_sys.add(part_name_2);
        // start system
        my_sys.load();
        my_sys.initialize();
        my_sys.start();
        auto p1 = my_sys.getParticipant(part_name_1);

        //check if inti priority are taking in concern
        p1.setInitPriority(23);
        ASSERT_EQ(p1.getInitPriority(), 23);
        auto check_p1 = my_sys.getParticipant(part_name_1);
        ASSERT_EQ(check_p1.getInitPriority(), 23);

        //check additional info
        check_p1.setAdditionalInfo("my_value", "this is the information i want");
        ASSERT_EQ(check_p1.getAdditionalInfo("my_value", ""), "this is the information i want");
        ASSERT_EQ(check_p1.getAdditionalInfo("my_value_does_not_exist_using_default", "does not exist"), "does not exist");
        

        auto state1 = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        auto p2 = my_sys.getParticipant(part_name_2);
        auto state2 = p2.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        ASSERT_EQ(state1, fep3::rpc::ParticipantState::running);
        ASSERT_EQ(state2, fep3::rpc::ParticipantState::running);

        // stop system
        my_sys.stop();
        state1 = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        state2 = p2.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        ASSERT_EQ(state1, fep3::rpc::ParticipantState::initialized);
        ASSERT_EQ(state2, fep3::rpc::ParticipantState::initialized);

        // pause system
        my_sys.pause();
        state1 = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        state2 = p2.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        ASSERT_EQ(state1, fep3::rpc::ParticipantState::paused);
        ASSERT_EQ(state2, fep3::rpc::ParticipantState::paused);

        // stop system
        my_sys.stop();
        state1 = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        state2 = p2.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        ASSERT_EQ(state1, fep3::rpc::ParticipantState::initialized);
        ASSERT_EQ(state2, fep3::rpc::ParticipantState::initialized);

        // shutdown event should be react
        ASSERT_ANY_THROW(
            my_sys.shutdown();
        );

        state1 = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        state2 = p2.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        ASSERT_EQ(state1, fep3::rpc::ParticipantState::initialized);
        ASSERT_EQ(state2, fep3::rpc::ParticipantState::initialized);

        // shutdown system
        my_sys.deinitialize();
        my_sys.unload();
        my_sys.shutdown();
        state1 = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        state2 = p2.getRPCComponentProxyByIID<fep3::rpc::IRPCParticipantStateMachine>()->getState();
        ASSERT_EQ(state1, fep3::rpc::ParticipantState::unreachable);
        ASSERT_EQ(state2, fep3::rpc::ParticipantState::unreachable);
    }
}

TEST(SystemLibrary, TestControlSystemNOK)
{
    const std::string sys_name = makePlatformDepName("system_under_test");

    {
        fep3::System my_sys(sys_name);
        // test with element that doesn't exist
        TestEventMonitor tem;
        my_sys.registerMonitoring(tem);
        my_sys.add("does_not_exist");
        bool caught = false;
        try
        {
            my_sys.setSystemState({ fep3::SystemAggregatedState::running }, std::chrono::milliseconds(500));
        }
        catch (std::runtime_error e)
        {
            std::string msg = e.what();
            ASSERT_STREQ("Participant does_not_exist is unreachable", msg.c_str());
            caught = true;
        }
        ASSERT_TRUE(caught);
        caught = false;
        ASSERT_TRUE(tem.waitForDone());

        // test with empty system
        my_sys.clear();
        try
        {
            my_sys.setSystemState(fep3::SystemAggregatedState::running, std::chrono::milliseconds(500));
        }
        catch (std::runtime_error e)
        {
            caught = true;
        }
        // exception is thrown ... because the current state cahnge is not possible and invalid
        ASSERT_TRUE(caught);
        // but logging message
        ASSERT_TRUE(tem.waitForDone());
        ASSERT_TRUE(tem._message.find("No participant has a statemachine") != std::string::npos);
        ASSERT_TRUE(tem._severity_level == fep3::logging::Severity::error);
        my_sys.unregisterMonitoring(tem);
    }
}

TEST(SystemLibrary, TestMonitorSystemOK)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const std::string part_name_1 = "participant1";

    const auto participant_names = std::vector<std::string>{ part_name_1 };
    
    // testing state monitor and log function
    {
        auto test_parts = createTestParticipants(participant_names, sys_name);
        TestEventMonitor tem;
        fep3::System my_sys(sys_name);
        my_sys.registerMonitoring(tem);
        my_sys.add(part_name_1);
        
        my_sys.load();

        my_sys.setSystemState(fep3::System::AggregatedState::running);

        EXPECT_TRUE(tem.waitFor("Successfully start", 5000));

        //this test is only possible if we have the logging system reactivatd
        ASSERT_TRUE(tem._participant_name.find(part_name_1) != std::string::npos);
        ASSERT_TRUE(tem._message.find("Successfully start") != std::string::npos);

        my_sys.setSystemState(fep3::System::AggregatedState::paused);

        EXPECT_TRUE(tem.waitFor("Successfully pausing", 5000));
        ASSERT_TRUE(tem._message.find("Successfully pausing") != std::string::npos);

        // unregister monitoring is important! 
        my_sys.unregisterMonitoring(tem);
       
    }
}

TEST(SystemLibrary, TestGetAClock)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const std::string part_name_1 = "participant1";
    const std::string part_name_2 = "participant2";

    const auto participant_names = std::vector<std::string>{ part_name_1, part_name_2};

    // testing state monitor and log function
    {
        auto test_parts = createTestParticipants(participant_names, sys_name);
        TestEventMonitor tem;
        fep3::System my_sys(sys_name);
        my_sys.registerMonitoring(tem);

        my_sys.add(part_name_1);
        my_sys.add(part_name_2);

        my_sys.load();

        /*
        ASSERT_NO_THROW(
            my_sys.configureTiming3DiscreteSteps(part_name_1, "", "");
        );*/

        ASSERT_NO_THROW(
            my_sys.configureTiming3ClockSyncOnlyInterpolation(part_name_1, "");
        );

        auto masters = my_sys.getCurrentTimingMasters();
        EXPECT_EQ(masters, std::vector<std::string>{ part_name_1 });

        ASSERT_NO_THROW(
            my_sys.setSystemState(fep3::System::AggregatedState::loaded);
            my_sys.setSystemState(fep3::System::AggregatedState::initialized);
            my_sys.setSystemState(fep3::System::AggregatedState::running);
            );
        
        auto part1 = my_sys.getParticipant(part_name_1).getRPCComponentProxyByIID<fep3::rpc::IRPCClockService>();
        auto time_part1 = part1->getTime("");
        auto part2 = my_sys.getParticipant(part_name_2).getRPCComponentProxyByIID<fep3::rpc::IRPCClockService>();
        auto time_part2 = part2->getTime("");

        EXPECT_LT(time_part1, time_part2);

        my_sys.setSystemState(fep3::System::AggregatedState::unloaded);

        // unregister monitoring is important! 
        my_sys.unregisterMonitoring(tem);

    }
}

TEST(SystemLibrary, getTimingPropertiesRealTime)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const std::string part_name = "participant";

    const auto participant_names = std::vector<std::string>{ part_name };

    auto test_parts = createTestParticipants(participant_names, sys_name);
    fep3::System my_sys(sys_name);
    my_sys.add(part_name);
    my_sys.load();

    auto props_part = my_sys.getParticipant(part_name).getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>()->getProperties("/");

    // equivalent of configureTiming3NoMaster
    const std::string string_type = fep3::PropertyType<std::string>::getTypeName();
    
    props_part->setProperty(FEP3_CLOCK_SERVICE_MAIN_CLOCK, FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME, string_type);
    props_part->setProperty(FEP3_SCHEDULER_SERVICE_SCHEDULER, FEP3_SCHEDULER_CLOCK_BASED, string_type);

    auto timing_properties = my_sys.getTimingProperties();
    ASSERT_EQ(timing_properties.size(), 1u);
    
    auto participant_iterator = timing_properties.begin();
    EXPECT_EQ(participant_iterator->first, "participant");

    const fep3::arya::IProperties& properties = *participant_iterator->second;
    auto property_names = properties.getPropertyNames();
    std::vector<std::string> expected_property_names = { FEP3_MAIN_CLOCK_PROPERTY, FEP3_SCHEDULER_PROPERTY };
    std::sort(property_names.begin(), property_names.end());
    std::sort(expected_property_names.begin(), expected_property_names.end());
    EXPECT_EQ(property_names, expected_property_names);

    EXPECT_EQ(properties.getProperty(FEP3_MAIN_CLOCK_PROPERTY), FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME);
    EXPECT_EQ(properties.getPropertyType(FEP3_MAIN_CLOCK_PROPERTY), string_type);

    EXPECT_EQ(properties.getProperty(FEP3_SCHEDULER_PROPERTY), FEP3_SCHEDULER_CLOCK_BASED);
    EXPECT_EQ(properties.getPropertyType(FEP3_SCHEDULER_PROPERTY), string_type);
}

TEST(SystemLibrary, getTimingPropertiesAFAP)
{
    const std::string sys_name = makePlatformDepName("system_under_test");
    const std::string master_part_name = "timing_master";
    const std::string slave_part_name = "timing_slave";

    const auto participant_names = std::vector<std::string>{ master_part_name, slave_part_name };

    auto test_parts = createTestParticipants(participant_names, sys_name);
    fep3::System my_sys(sys_name);
    my_sys.add(master_part_name);
    my_sys.add(slave_part_name);
    my_sys.load();

    auto props_master = my_sys.getParticipant(master_part_name).getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>()->getProperties("/");
    auto props_slave = my_sys.getParticipant(slave_part_name).getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>()->getProperties("/");

    const std::string string_type = fep3::PropertyType<std::string>::getTypeName();
    const std::string double_type = fep3::PropertyType<double>::getTypeName();
    const std::string int32_type = fep3::PropertyType<int32_t>::getTypeName();

    // equivalent of configureTiming3AFAP("timing_master", "50")
    props_master->setProperty(FEP3_CLOCKSYNC_SERVICE_CONFIG_TIMING_MASTER, master_part_name, string_type);
    props_master->setProperty(FEP3_SCHEDULER_SERVICE_SCHEDULER, FEP3_SCHEDULER_CLOCK_BASED, string_type);
    props_master->setProperty(FEP3_CLOCK_SERVICE_MAIN_CLOCK, FEP3_CLOCK_LOCAL_SYSTEM_SIM_TIME, string_type);
    props_master->setProperty(FEP3_CLOCK_SERVICE_CLOCK_SIM_TIME_TIME_FACTOR, "0.0", double_type);
    props_master->setProperty(FEP3_CLOCK_SERVICE_CLOCK_SIM_TIME_CYCLE_TIME, "50", int32_type);

    props_slave->setProperty(FEP3_CLOCKSYNC_SERVICE_CONFIG_TIMING_MASTER, master_part_name, string_type);
    props_slave->setProperty(FEP3_SCHEDULER_SERVICE_SCHEDULER, FEP3_SCHEDULER_CLOCK_BASED, string_type);
    props_slave->setProperty(FEP3_CLOCK_SERVICE_MAIN_CLOCK, FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE, string_type);

    auto timing_properties = my_sys.getTimingProperties();
    ASSERT_EQ(timing_properties.size(), 2u);

    auto master_iterator = timing_properties.begin();
    EXPECT_EQ(master_iterator->first, master_part_name);

    const fep3::arya::IProperties& master_properties = *master_iterator->second;
    auto master_property_names = master_properties.getPropertyNames();
    std::vector<std::string> expected_master_property_names = { FEP3_TIMING_MASTER_PROPERTY, FEP3_SCHEDULER_PROPERTY,
        FEP3_MAIN_CLOCK_PROPERTY, FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY, FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY,
        FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY, FEP3_TIME_UPDATE_TIMEOUT_PROPERTY };

    std::sort(master_property_names.begin(), master_property_names.end());
    std::sort(expected_master_property_names.begin(), expected_master_property_names.end());
    EXPECT_EQ(master_property_names, expected_master_property_names);

    EXPECT_EQ(master_properties.getProperty(FEP3_TIMING_MASTER_PROPERTY), master_part_name);
    EXPECT_EQ(master_properties.getPropertyType(FEP3_TIMING_MASTER_PROPERTY), string_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_SCHEDULER_PROPERTY), FEP3_SCHEDULER_CLOCK_BASED);
    EXPECT_EQ(master_properties.getPropertyType(FEP3_SCHEDULER_PROPERTY), string_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_MAIN_CLOCK_PROPERTY), FEP3_CLOCK_LOCAL_SYSTEM_SIM_TIME);
    EXPECT_EQ(master_properties.getPropertyType(FEP3_MAIN_CLOCK_PROPERTY), string_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), "0.0");
    EXPECT_EQ(master_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), double_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), "50");
    EXPECT_EQ(master_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), std::to_string(FEP3_SLAVE_SYNC_CYCLE_TIME_DEFAULT_VALUE));
    EXPECT_EQ(master_properties.getPropertyType(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), std::to_string(FEP3_TIME_UPDATE_TIMEOUT_DEFAULT_VALUE));
    EXPECT_EQ(master_properties.getPropertyType(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), int32_type);

    auto slave_iterator = std::next(timing_properties.begin());
    EXPECT_EQ(slave_iterator->first, slave_part_name);

    const fep3::arya::IProperties& slave_properties = *slave_iterator->second;
    auto slave_property_names = slave_properties.getPropertyNames();
    std::vector<std::string> expected_slave_property_names = { FEP3_TIMING_MASTER_PROPERTY, FEP3_SCHEDULER_PROPERTY,
        FEP3_MAIN_CLOCK_PROPERTY, FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY, FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY,
        FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY, FEP3_TIME_UPDATE_TIMEOUT_PROPERTY };

    std::sort(slave_property_names.begin(), slave_property_names.end());
    std::sort(expected_slave_property_names.begin(), expected_slave_property_names.end());
    EXPECT_EQ(slave_property_names, expected_slave_property_names);

    EXPECT_EQ(slave_properties.getProperty(FEP3_TIMING_MASTER_PROPERTY), master_part_name);
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_TIMING_MASTER_PROPERTY), string_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_SCHEDULER_PROPERTY), FEP3_SCHEDULER_CLOCK_BASED);
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_SCHEDULER_PROPERTY), string_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_MAIN_CLOCK_PROPERTY), FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE);
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_MAIN_CLOCK_PROPERTY), string_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), std::to_string(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), double_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), std::to_string(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), std::to_string(FEP3_SLAVE_SYNC_CYCLE_TIME_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), std::to_string(FEP3_TIME_UPDATE_TIMEOUT_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), int32_type);
}

TEST(SystemLibrary, getTimingPropertiesDiscrete)
{
    const std::string sys_name = makePlatformDepName("tested_system");
    const std::string master_part_name = "timing_master";
    const std::string slave_part_name = "timing_slave";

    const auto participant_names = std::vector<std::string>{ master_part_name, slave_part_name };

    auto test_parts = createTestParticipants(participant_names, sys_name);
    fep3::System my_sys(sys_name);
    my_sys.add(master_part_name);
    my_sys.add(slave_part_name);
    my_sys.load();

    auto props_master = my_sys.getParticipant(master_part_name).getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>()->getProperties("/");
    auto props_slave = my_sys.getParticipant(slave_part_name).getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>()->getProperties("/");

    const std::string string_type = fep3::PropertyType<std::string>::getTypeName();
    const std::string double_type = fep3::PropertyType<double>::getTypeName();
    const std::string int32_type = fep3::PropertyType<int32_t>::getTypeName();

    // equivalent of configureTiming3ClockSyncOnlyDiscrete(master_part_name, "20")
    props_master->setProperty(FEP3_CLOCKSYNC_SERVICE_CONFIG_TIMING_MASTER, master_part_name, string_type);
    props_master->setProperty(FEP3_SCHEDULER_SERVICE_SCHEDULER, FEP3_SCHEDULER_CLOCK_BASED, string_type);
    props_master->setProperty(FEP3_CLOCK_SERVICE_MAIN_CLOCK, FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME, string_type);

    props_slave->setProperty(FEP3_CLOCKSYNC_SERVICE_CONFIG_TIMING_MASTER, master_part_name, string_type);
    props_slave->setProperty(FEP3_SCHEDULER_SERVICE_SCHEDULER, FEP3_SCHEDULER_CLOCK_BASED, string_type);
    props_slave->setProperty(FEP3_CLOCK_SERVICE_MAIN_CLOCK, FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE, string_type);
    props_slave->setProperty(FEP3_CLOCKSYNC_SERVICE_CONFIG_SLAVE_SYNC_CYCLE_TIME,"20" , int32_type);

    auto timing_properties = my_sys.getTimingProperties();
    ASSERT_EQ(timing_properties.size(), 2u);

    auto master_iterator = timing_properties.begin();
    EXPECT_EQ(master_iterator->first, master_part_name);

    const fep3::arya::IProperties& master_properties = *master_iterator->second;
    auto master_property_names = master_properties.getPropertyNames();
    std::vector<std::string> expected_master_property_names = { FEP3_TIMING_MASTER_PROPERTY, FEP3_SCHEDULER_PROPERTY,
        FEP3_MAIN_CLOCK_PROPERTY, FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY, FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY,
        FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY, FEP3_TIME_UPDATE_TIMEOUT_PROPERTY };

    std::sort(master_property_names.begin(), master_property_names.end());
    std::sort(expected_master_property_names.begin(), expected_master_property_names.end());
    EXPECT_EQ(master_property_names, expected_master_property_names);

    EXPECT_EQ(master_properties.getProperty(FEP3_TIMING_MASTER_PROPERTY), master_part_name);
    EXPECT_EQ(master_properties.getPropertyType(FEP3_TIMING_MASTER_PROPERTY), string_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_SCHEDULER_PROPERTY), FEP3_SCHEDULER_CLOCK_BASED);
    EXPECT_EQ(master_properties.getPropertyType(FEP3_SCHEDULER_PROPERTY), string_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_MAIN_CLOCK_PROPERTY), FEP3_CLOCK_LOCAL_SYSTEM_REAL_TIME);
    EXPECT_EQ(master_properties.getPropertyType(FEP3_MAIN_CLOCK_PROPERTY), string_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), std::to_string(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_DEFAULT_VALUE));
    EXPECT_EQ(master_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), double_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), std::to_string(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_DEFAULT_VALUE));
    EXPECT_EQ(master_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), std::to_string(FEP3_SLAVE_SYNC_CYCLE_TIME_DEFAULT_VALUE));
    EXPECT_EQ(master_properties.getPropertyType(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(master_properties.getProperty(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), std::to_string(FEP3_TIME_UPDATE_TIMEOUT_DEFAULT_VALUE));
    EXPECT_EQ(master_properties.getPropertyType(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), int32_type);

    auto slave_iterator = std::next(timing_properties.begin());
    EXPECT_EQ(slave_iterator->first, slave_part_name);

    const fep3::arya::IProperties& slave_properties = *slave_iterator->second;
    auto slave_property_names = slave_properties.getPropertyNames();
    std::vector<std::string> expected_slave_property_names = { FEP3_TIMING_MASTER_PROPERTY, FEP3_SCHEDULER_PROPERTY,
        FEP3_MAIN_CLOCK_PROPERTY, FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY, FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY,
        FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY, FEP3_TIME_UPDATE_TIMEOUT_PROPERTY };

    std::sort(slave_property_names.begin(), slave_property_names.end());
    std::sort(expected_slave_property_names.begin(), expected_slave_property_names.end());
    EXPECT_EQ(slave_property_names, expected_slave_property_names);

    EXPECT_EQ(slave_properties.getProperty(FEP3_TIMING_MASTER_PROPERTY), master_part_name);
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_TIMING_MASTER_PROPERTY), string_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_SCHEDULER_PROPERTY), FEP3_SCHEDULER_CLOCK_BASED);
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_SCHEDULER_PROPERTY), string_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_MAIN_CLOCK_PROPERTY), FEP3_CLOCK_SLAVE_MASTER_ONDEMAND_DISCRETE);
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_MAIN_CLOCK_PROPERTY), string_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), std::to_string(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_TIME_FACTOR_PROPERTY), double_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), std::to_string(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_CLOCK_SIM_TIME_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), "20");
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_SLAVE_SYNC_CYCLE_TIME_PROPERTY), int32_type);

    EXPECT_EQ(slave_properties.getProperty(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), std::to_string(FEP3_TIME_UPDATE_TIMEOUT_DEFAULT_VALUE));
    EXPECT_EQ(slave_properties.getPropertyType(FEP3_TIME_UPDATE_TIMEOUT_PROPERTY), int32_type);
}

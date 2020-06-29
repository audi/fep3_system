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


class TestEventMonitor : public fep3::IEventMonitor
{
public:
    void onLog(std::chrono::milliseconds,
        fep3::logging::Severity severity_level,
        const std::string& participant_name,
        const std::string& logger_name, //depends on the Category ... 
        const std::string& message) override
    {
        std::unique_lock<std::mutex> lk(_cv_m);
        _severity_level = severity_level;
        _participant_name = participant_name;
        _logger_name = logger_name;
        _message = message;
        _done = true;
    }

    bool waitForDone(timestamp_t timeout_ms = 2000)
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

    fep3::logging::Severity _severity_level;
    std::string _participant_name;
    std::string _message;
    std::string _logger_name;
    std::vector<fep3::rpc::ParticipantState> _states;
    std::string _new_name;
private:
    std::mutex _cv_m;
    std::atomic_bool _done{ false };
};


TEST(SystemLibrary, TestStateProxy)
{
    using namespace fep3;
    System systm(makePlatformDepName("Blackbox"));
    TestParticipants participants;


    std::string participant1_name = "Participant1";
    ASSERT_NO_THROW(
          participants = createTestParticipants({ participant1_name }, systm.getSystemName());
    );

    ASSERT_NO_THROW(
        systm.add(participant1_name);
    );

    auto p1 = systm.getParticipant(participant1_name);
    auto sm = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>();

    ASSERT_TRUE(p1);
    ASSERT_TRUE(sm);
   
    ASSERT_EQ(sm->getRPCDefaultName(), rpc::getRPCDefaultName<fep3::rpc::IRPCParticipantStateMachine>());
    ASSERT_EQ(sm->getRPCIID(), rpc::getRPCIID<fep3::rpc::IRPCParticipantStateMachine>());

    ASSERT_EQ(sm->getState(), rpc::ParticipantState::unloaded);

    sm->load();
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::loaded);

    sm->initialize();
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::initialized);

    sm->start();
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::running);

    sm->stop();
    // beauty sleep to secure state is reached
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::initialized);

    sm->pause();
    // beauty sleep to secure state is reached
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::paused);

    sm->stop();
    // beauty sleep to secure state is reached
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::initialized);

    sm->deinitialize();
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::loaded);

    sm->unload();
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::unloaded);

    sm->shutdown();
    ASSERT_EQ(sm->getState(), rpc::ParticipantState::unreachable);
}

bool contains(const std::vector<std::string>& list_of_components, const std::vector<std::string>& list_of_components_expected)
{
    size_t found = 0;
    for (const auto& current_search : list_of_components_expected)
    {
        for (const auto& current_component : list_of_components)
        {
            if (current_component == current_search)
            {
                found++;
                break;
            }
        }
    }
    return (found == list_of_components_expected.size());
}

TEST(SystemLibrary, TestParticpantInfo)
{
    using namespace fep3;
    System systm(makePlatformDepName("Blackbox"));
    TestParticipants participants;


    std::string participant1_name = "Participant1";
    ASSERT_NO_THROW(
        participants = createTestParticipants({ participant1_name }, systm.getSystemName());
    );

    ASSERT_NO_THROW(
        systm.add(participant1_name);
    );

    auto p1 = systm.getParticipant(participant1_name);
    auto pi = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantInfo>();
 
    ASSERT_EQ(pi->getRPCDefaultName(), rpc::getRPCDefaultName<fep3::rpc::IRPCParticipantInfo>());
    ASSERT_EQ(pi->getRPCIID(), rpc::getRPCIID<fep3::rpc::IRPCParticipantInfo>());
    ASSERT_EQ(pi->getSystemName(), systm.getSystemName());
    ASSERT_EQ(pi->getName(), participant1_name);

    ASSERT_TRUE(!(pi->getRPCComponentInterfaceDefinition(rpc::getRPCDefaultName<fep3::rpc::IRPCParticipantInfo>(), rpc::getRPCIID<fep3::rpc::IRPCParticipantInfo>()).empty()));

    ASSERT_TRUE(contains(pi->getRPCComponents(), { rpc::getRPCDefaultName<fep3::rpc::IRPCParticipantInfo>(),
                                       rpc::getRPCDefaultName<fep3::rpc::IRPCParticipantStateMachine>()}));
}

/*
TEST(SystemLibrary, TestDataRegistry)
{
    System systm("Blackbox");
    cTestBaseModule mod;
    ASSERT_EQ(a_util::result::SUCCESS, mod.Create("Participant1"));
    // register some random signals
    mod.GetSignalRegistry()->RegisterSignalDescription("files/timing_example.description");
    handle_t some_handle;
    mod.GetSignalRegistry()->RegisterSignal(fep::cUserSignalOptions("tFEP_Examples_ObjectState", SD_Output), some_handle);
    // trigger participant into state FS_READY
    mod.GetStateMachine()->InitializeEvent();
    // verify signals are given out by DRProxy
    systm.add(mod.GetName());
    auto p1 = systm.getParticipant(mod.GetName());
    auto dr = p1.getRPCComponentProxy<fep::rpc::IRPCDataRegistry>();
  
    ASSERT_STREQ(dr->getRPCDefaultName(), "data_registry");
    ASSERT_STREQ(dr->getRPCIID(), "data_registry.iid");
    std::vector<std::string> ref_vec{ "tFEP_Examples_ObjectState" };
    ASSERT_TRUE(dr->getSignalsOut() == ref_vec);
    ASSERT_TRUE(dr->getSignalsIn().empty());
    ASSERT_STREQ(dr->getStreamType("tFEP_Examples_ObjectState").getMetaTypeName(), "ddl");
}*/

TEST(SystemLibrary, TestProxiesNOK)
{
    using namespace fep3;
    System systm(makePlatformDepName("Blackbox"));

    TestEventMonitor tem;
    systm.registerMonitoring(tem);
    systm.add("does_not_exist");

    auto p1 = systm.getParticipant("does_not_exist");
    bool caught = false;
    try
    {
        p1.getRPCComponentProxyByIID<rpc::IRPCParticipantStateMachine>();
    }
    catch (std::runtime_error e)
    {
        std::string msg = e.what();
        caught = true;
        ASSERT_STREQ("Participant does_not_exist is unreachable", e.what());
    }

    ASSERT_TRUE(caught);
    tem.waitForDone(3000);

    ASSERT_EQ(tem._severity_level, logging::Severity::fatal);
    ASSERT_EQ(tem._participant_name, "does_not_exist");
    ASSERT_EQ(tem._logger_name, systm.getSystemName());
    ASSERT_TRUE(tem._message.find("Participant does_not_exist is unreachable") != std::string::npos);
    {
        systm.clear();
        TestParticipants participants;
        std::string participant1_name = "Participant_for_test";
        ASSERT_NO_THROW(
            participants = createTestParticipants({ participant1_name }, systm.getSystemName());
        );

        ASSERT_NO_THROW(
            systm.add(participant1_name);
        );

        p1 = systm.getParticipant(participant1_name);
        auto sm = p1.getRPCComponentProxy<fep3::rpc::IRPCParticipantStateMachine>();
    }

    systm.unregisterMonitoring(tem);
}
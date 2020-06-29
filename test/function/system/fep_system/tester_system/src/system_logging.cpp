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
#include <string>
#include "fep_test_common.h"
#include <a_util/logging.h>
#include <a_util/process.h>


struct TestEventMonLog : public fep3::IEventMonitor
{
    TestEventMonLog()
    {
        _logcount = 0;
    }
    ~TestEventMonLog()
    {
    }
    void onLog(std::chrono::milliseconds /*log_time*/,
        fep3::logging::Severity severity_level,
        const std::string& participant_name,
        const std::string& logger_name,
        const std::string& message) override
    {
        if (logger_name == "Testelement.element")
        {
            _messages[severity_level].push_back(logger_name + "---" + participant_name + "---" + message);
            _logcount++;
        }
    }
    std::map<fep3::logging::Severity, std::vector<std::string>> _messages;
    std::atomic<int32_t> _logcount;
};

TEST(SystemLibrary, TestParticpantInfo)
{
    auto sys_name = makePlatformDepName("test_system");
    const auto participant_names = std::vector<std::string>{ "test_participant1", "test_participant2" };
    const TestParticipants test_parts = createTestParticipants(participant_names, sys_name);

    fep3::System my_system(sys_name);
    for (const auto& part : test_parts)
    {
        my_system.add(part.first);
    }
    TestEventMonLog test_log;
    my_system.registerMonitoring(test_log);
    my_system.load();
    my_system.initialize();

    int try_count = 20;
    while (test_log._logcount < 16 && try_count > 0)
    {
        a_util::system::sleepMilliseconds(200);
        try_count--;
    }
    my_system.unregisterMonitoring(test_log);

    EXPECT_EQ(test_log._messages[fep3::logging::Severity::debug].size(), 0);
    EXPECT_EQ(test_log._messages[fep3::logging::Severity::info].size(), 4);
    EXPECT_EQ(test_log._messages[fep3::logging::Severity::warning].size(), 4);
    EXPECT_EQ(test_log._messages[fep3::logging::Severity::error].size(), 4);
    EXPECT_EQ(test_log._messages[fep3::logging::Severity::fatal].size(), 4);

    for (const auto& content_log : test_log._messages[fep3::logging::Severity::info])
    {
        EXPECT_TRUE(content_log.find("initializing") != std::string::npos ||
            content_log.find("loading") != std::string::npos || 
            content_log.find(sys_name) != std::string::npos);
    }
    for (const auto& content_log : test_log._messages[fep3::logging::Severity::warning])
    {
        EXPECT_TRUE(content_log.find("initializing") != std::string::npos ||
            content_log.find("loading") != std::string::npos);
    }
    for (const auto& content_log : test_log._messages[fep3::logging::Severity::error])
    {
        EXPECT_TRUE(content_log.find("initializing") != std::string::npos ||
            content_log.find("loading") != std::string::npos);
    }
    for (const auto& content_log : test_log._messages[fep3::logging::Severity::fatal])
    {
        EXPECT_TRUE(content_log.find("initializing") != std::string::npos ||
            content_log.find("loading") != std::string::npos);
    }
}


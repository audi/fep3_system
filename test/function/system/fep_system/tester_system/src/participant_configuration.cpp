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
#include <gmock/gmock.h>
#include <fep_system/fep_system.h>
#include "participant_configuration_helper.hpp"
#include "fep_test_common.h"
#include <fep3/components/configuration/propertynode.h>
#include <fep_system/mock/event_monitor.h>
#include "../../../../../function/utils/common/gtest_asserts.h"


using EventMonitorMock = testing::NiceMock<fep3::mock::EventMonitor>;

TEST(ParticipantConfiguration, TestProxyConfigGetter)
{
    using namespace fep3;
    System system_under_test(makePlatformDepName("Blackbox"));

    auto parts = createTestParticipants({ "Participant1_configuration_test" }, system_under_test.getSystemName());
    system_under_test.add("Participant1_configuration_test");

    auto p1 = system_under_test.getParticipant("Participant1_configuration_test");

    auto config = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
    ASSERT_TRUE(static_cast<bool>(config));

    auto& part = parts["Participant1_configuration_test"];
    auto config_service = part->_part.getComponent<IConfigurationService>();

    auto props = std::make_shared<NativePropertyNode>("deeper");
    props->setChild(makeNativePropertyNode("bool_value", false));
    props->setChild(makeNativePropertyNode("int_value", 1));
    props->setChild(makeNativePropertyNode("double_value", double(0.1)));
    props->setChild(makeNativePropertyNode("string_value", std::string("test_value")));
    config_service->registerNode(props);

    bool bool_value = true;
    testGetter(*config_service, config.getInterface(), bool_value, "bool_value", "deeper");
    bool_value = false;
    testGetter(*config_service, config.getInterface(), bool_value, "bool_value", "deeper");

    int32_t int_value = 3456;
    testGetter(*config_service, config.getInterface(), int_value, "int_value", "deeper");

    double double_value = 1.0;
    testGetter(*config_service, config.getInterface(), double_value, "double_value", "deeper");

    std::string string_val = "this_is_may_string";
    testGetter(*config_service, config.getInterface(), string_val, "string_value", "deeper");
}


TEST(ParticipantConfiguration, TestProxyConfigSetter)
{
    using namespace fep3;
    System system_under_test(makePlatformDepName("Blackbox"));

    auto parts = createTestParticipants({ "Participant1_configuration_test" }, system_under_test.getSystemName());
    system_under_test.add("Participant1_configuration_test");

    auto p1 = system_under_test.getParticipant("Participant1_configuration_test");

    auto config = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
    ASSERT_TRUE(static_cast<bool>(config));

    auto& part = parts["Participant1_configuration_test"];
    auto config_service = part->_part.getComponent<IConfigurationService>();

    auto props = std::make_shared<NativePropertyNode>("deeper");
    props->setChild(makeNativePropertyNode("bool_value", false));
    props->setChild(makeNativePropertyNode("int_value", 1));
    props->setChild(makeNativePropertyNode("double_value", double(0.1)));
    props->setChild(makeNativePropertyNode("string_value", std::string("test_value")));
    config_service->registerNode(props);

    bool bool_value = true;
    testSetter(*config_service, config.getInterface(), bool_value, false, "bool_value", "deeper");
    bool_value = false;
    testSetter(*config_service, config.getInterface(), bool_value, true, "bool_value", "deeper");

    int32_t int_value = 3456;
    testSetter(*config_service, config.getInterface(), int_value, 1, "int_value", "deeper");

    double double_value = 1.0;
    testSetter(*config_service, config.getInterface(), double_value, 0.0, "double_value", "deeper");

    std::string string_val = "this_is_may_string";
    testSetter(*config_service, config.getInterface(), string_val,
        std::string("init_val"), std::string("string_value"), "deeper");
}

/** @brief Test whether a system property may be set for a system consisting of multiple participants
* using the fep system setSystemProperty functionality.
*/
TEST(ParticipantConfiguration, TestSetSystemProperty)
{
	using namespace fep3;
	using namespace testing;
	
	System system_under_test(makePlatformDepName("Blackbox"));
	
	auto parts = createTestParticipants({ "part1", "part2", "part3" }, system_under_test.getSystemName());
	system_under_test.add("part1");
	system_under_test.add("part2");
	system_under_test.add("part3");

	const auto config_service1 = parts["part1"]->_part.getComponent<IConfigurationService>();
	ASSERT_FEP3_NOERROR(config_service1->createSystemProperty("test", "string", ""));
	const auto config_service2 = parts["part2"]->_part.getComponent<IConfigurationService>();
	ASSERT_FEP3_NOERROR(config_service2->createSystemProperty("test", "string", ""));
	const auto config_service3 = parts["part3"]->_part.getComponent<IConfigurationService>();
	ASSERT_FEP3_NOERROR(config_service3->createSystemProperty("test", "string", ""));

	system_under_test.setSystemProperty("test", "string", "value");
	
	ASSERT_TRUE(getPropertyValue<std::string>(*config_service1, "system/test").has_value());
	ASSERT_EQ("value", getPropertyValue<std::string>(*config_service1, "system/test").value());
	ASSERT_TRUE(getPropertyValue<std::string>(*config_service2, "system/test").has_value());
	ASSERT_EQ("value", getPropertyValue<std::string>(*config_service2, "system/test").value());
	ASSERT_TRUE(getPropertyValue<std::string>(*config_service3, "system/test").has_value());
	ASSERT_EQ("value", getPropertyValue<std::string>(*config_service3, "system/test").value());

	ASSERT_FEP3_NOERROR(config_service1->createSystemProperty("nested/path/property", "int", ""));
	ASSERT_FEP3_NOERROR(config_service2->createSystemProperty("nested/path/property", "int", ""));
	ASSERT_FEP3_NOERROR(config_service3->createSystemProperty("nested/path/property", "int", ""));
	
	system_under_test.setSystemProperty("nested/path/property", "int", "123");

	ASSERT_TRUE(getPropertyValue<int>(*config_service1, "system/nested/path/property").has_value());
	ASSERT_EQ(123, getPropertyValue<int>(*config_service1, "system/nested/path/property").value());
	ASSERT_TRUE(getPropertyValue<int>(*config_service2, "system/nested/path/property").has_value());
	ASSERT_EQ(123, getPropertyValue<int>(*config_service2, "system/nested/path/property").value());
	ASSERT_TRUE(getPropertyValue<int>(*config_service3, "system/nested/path/property").has_value());
	ASSERT_EQ(123, getPropertyValue<int>(*config_service3, "system/nested/path/property").value());
}

/** @brief Test whether a system property may be created for a system consisting of multiple participants
* even if setting the property fails for some participants of the system.
* Setting for part2 fails because the system property does not exist.
* Setting for part3 fails because the participant does not exist.
*/
TEST(ParticipantConfiguration, TestSetSystemPropertyPartialSuccess)
{
	using namespace fep3;
	using namespace testing;

	System system_under_test(makePlatformDepName("Blackbox"));

	EventMonitorMock event_monitor_mock;

	system_under_test.registerMonitoring(event_monitor_mock);

	EXPECT_CALL(event_monitor_mock, onLog(_, _, _, _, _)).Times(AnyNumber());
	EXPECT_CALL(event_monitor_mock, onLog(_, logging::Severity::warning, _, _, MatchesRegex(".*could not be set for.* part2, part3.*"))).Times(1);

	auto parts = createTestParticipants({ "part1", "part2" }, system_under_test.getSystemName());
	system_under_test.add("part1");
	system_under_test.add("part2");
	system_under_test.add("part3");

	const auto config_service1 = parts["part1"]->_part.getComponent<IConfigurationService>();
	ASSERT_FEP3_NOERROR(config_service1->createSystemProperty("double/property", "double", ""));

	system_under_test.setSystemProperty("/double/property/", "double", "1.23");
	
	ASSERT_TRUE(getPropertyValue<double>(*config_service1, "system/double/property").has_value());
	ASSERT_EQ(1.23, getPropertyValue<double>(*config_service1, "system/double/property").value());
}

/** @brief Test whether a warning is logged if setting a system property fails due to an invalid path.
*/
TEST(ParticipantConfiguration, TestSetSystemPropertyFailureInvalidPath)
{
	using namespace fep3;
	using namespace testing;

	System system_under_test(makePlatformDepName("Blackbox"));

	EventMonitorMock event_monitor_mock;

	system_under_test.registerMonitoring(event_monitor_mock);

	EXPECT_CALL(event_monitor_mock, onLog(_, _, _, _, _)).Times(AnyNumber());
	EXPECT_CALL(event_monitor_mock, onLog(_, logging::Severity::warning, _, _, MatchesRegex(".*could not be set for.* part1, part2.*"))).Times(1);

	auto parts = createTestParticipants({ "part1", "part2" }, system_under_test.getSystemName());
	system_under_test.add("part1");
	system_under_test.add("part2");

	system_under_test.setSystemProperty("", "type", "value");
}

TEST(ParticipantConfiguration, TestProxyConfigArraySetter)
{
    using namespace fep3;
    System system_under_test(makePlatformDepName("Blackbox"));

    auto parts = createTestParticipants({ "Participant1_configuration_test" }, system_under_test.getSystemName());
    system_under_test.add("Participant1_configuration_test");

    auto p1 = system_under_test.getParticipant("Participant1_configuration_test");

    auto config = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
    ASSERT_TRUE(static_cast<bool>(config));

    auto& part = parts["Participant1_configuration_test"];
    auto config_service = part->_part.getComponent<IConfigurationService>();

    auto props = std::make_shared<NativePropertyNode>("deeper");
    props->setChild(makeNativePropertyNode("test_bool_array", std::vector<bool>()));
    props->setChild(makeNativePropertyNode("test_int_array", std::vector<int32_t>()));
    props->setChild(makeNativePropertyNode("test_double_array", std::vector<double>()));
    props->setChild(makeNativePropertyNode("test_string_array", std::vector<std::string>()));
    config_service->registerNode(props);

    std::vector<bool> bool_value_array = { true, false, true };
    testSetter<std::vector<bool>>(*config_service,
        config.getInterface(), bool_value_array, { false, true, false }, "test_bool_array", "deeper");

    std::vector<int32_t> int_value_array = { 3456, 2, 3, 4};
    testSetter<std::vector<int32_t>>(*config_service,
        config.getInterface(), int_value_array, { 1, 2 }, "test_int_array", "deeper");

    std::vector<double> double_value_array = { 1.0, 2.0 };
    testSetter<std::vector<double>>(*config_service,
        config.getInterface(), double_value_array, { 0.0, 0.0 }, "test_double_array", "deeper");

    std::vector<std::string> string_val_array = { "this_is_may_string", "test2", "test3" };
    testSetter<std::vector<std::string>>(*config_service,
        config.getInterface(), string_val_array, { "init_val", "another_val" }, "test_string_array", "deeper");
}

/** @brief Test whether the system lib refuses to support the '.' property syntax.
*   This means the system shall not:
*   - set a value 
*   - get the value
*   - get the type
*   of properties which use the '.' syntax.
*   Properties have to use a '/' delimiter instead of '.'.
*   @req_id FEPSDK-2171
*/
TEST(cTesterParticipantConfiguration, TestInvalidPropertySyntax)
{
    using namespace fep3;
    System system_under_test(makePlatformDepName("Blackbox"));

    auto parts = createTestParticipants({ "Participant1_configuration_test" }, system_under_test.getSystemName());
    system_under_test.add("Participant1_configuration_test");

    auto p1 = system_under_test.getParticipant("Participant1_configuration_test");

    auto config = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
    ASSERT_TRUE(static_cast<bool>(config));

    auto& part = parts["Participant1_configuration_test"];
    auto config_service = part->_part.getComponent<IConfigurationService>();

    auto props = std::make_shared<NativePropertyNode>("test_node");
    props->setChild(makeNativePropertyNode("bool_value", false));
    props->setChild(makeNativePropertyNode("int_value", 1));
    props->setChild(makeNativePropertyNode("double_value", double(0.1)));
    props->setChild(makeNativePropertyNode("string_value", std::string("test_value")));
    config_service->registerNode(props);

    // we have to set already existing properties because not existing properties may not be set using the system lib
    
    bool bool_value = true;
    testSetGetInvalidFormat(config.getInterface(), bool_value, "test_node.test_bool");

    int32_t int_value = 3456;
    testSetGetInvalidFormat(config.getInterface(), int_value, "test_node.int_value");

    double double_value = 1.0;
    testSetGetInvalidFormat(config.getInterface(), double_value, "test_node.double_value");

    std::string string_value = "test_value";
    testSetGetInvalidFormat(config.getInterface(), string_value, "test_node.string_value");
}

/**
 * @brief Test whether root properties may be set by setting a property with a fully specified path.
 * @req_id FEPSDK-2171
 */
TEST(cTesterParticipantConfiguration, TestRootPropertySetter)
{
    using namespace fep3;
    System system_under_test(makePlatformDepName("Blackbox"));

    auto parts = createTestParticipants({ "Participant1_configuration_test" }, system_under_test.getSystemName());
    system_under_test.add("Participant1_configuration_test");

    auto p1 = system_under_test.getParticipant("Participant1_configuration_test");

    auto config = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
    ASSERT_TRUE(static_cast<bool>(config));

    auto& part = parts["Participant1_configuration_test"];
    auto config_service = part->_part.getComponent<IConfigurationService>();

    auto props = std::make_shared<NativePropertyNode>("test_node");
    props->setChild(makeNativePropertyNode("bool_value", false));

    int32_t initial_int_value = 4;
    auto prop_sub = makeNativePropertyNode("int_value_sub", initial_int_value);
    props->setChild(prop_sub);
    prop_sub->setChild(makeNativePropertyNode("child_int_value", initial_int_value));

    config_service->registerNode(props);

    bool bool_value = true, initial_bool_value = false;
    testSetGetRoot(config.getInterface(), initial_bool_value, bool_value, "/test_node", "bool_value");

    bool_value = false;
    initial_bool_value = true;
    testSetGetRoot(config.getInterface(), initial_bool_value, bool_value, "", "test_node/bool_value");

    int32_t int_value = 5;
    initial_int_value = 4;
    testSetGetRoot(config.getInterface(), initial_int_value, int_value, "/test_node/int_value_sub", "child_int_value");

    int_value = 10;
    initial_int_value = 5;
    testSetGetRoot(config.getInterface(), initial_int_value, int_value, "/", "test_node/int_value_sub/child_int_value");

    int_value = 20;
    initial_int_value = 10;
    testSetGetRoot(config.getInterface(), initial_int_value, int_value, "test_node", "int_value_sub/child_int_value");

}

/**
 * @brief Test whether root properties may be set by setting a property with a fully specified path.
 * @req_id FEPSDK-2164
 */
TEST(cTesterParticipantConfiguration, TestProxyConfigPropNotExistsNoCreate)
{
    using namespace fep3;
    System system_under_test(makePlatformDepName("Blackbox"));

    auto parts = createTestParticipants({ "Participant1_configuration_test" }, system_under_test.getSystemName());
    system_under_test.add("Participant1_configuration_test");

    auto p1 = system_under_test.getParticipant("Participant1_configuration_test");

    auto config = p1.getRPCComponentProxyByIID<fep3::rpc::IRPCConfiguration>();
    ASSERT_TRUE(static_cast<bool>(config));

    auto& part = parts["Participant1_configuration_test"];
    auto config_service = part->_part.getComponent<IConfigurationService>();

    auto prop_opt = getPropertyValue<std::string>(*config_service, "test_does_not_exists");
    //check if it does not exists
    ASSERT_FALSE(prop_opt);

    auto props = config->getProperties("/");
    auto retval = props->getProperty("test_does_not_exists");
    auto retval_type = props->getPropertyType("test_does_not_exists");
    //check if empty
    ASSERT_EQ(retval, std::string());
    ASSERT_EQ(retval_type, std::string());
    props->setProperty("test_does_not_exists", "value", "string");
    //check if still empty
    retval = props->getProperty("test_does_not_exists");
    retval_type = props->getPropertyType("test_does_not_exists");
    ASSERT_EQ(retval, std::string());
    ASSERT_EQ(retval_type, std::string());

    prop_opt = getPropertyValue<std::string>(*config_service, "test_does_not_exists");
    //check if it does not exists
    ASSERT_FALSE(prop_opt);
}


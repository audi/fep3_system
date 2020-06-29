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
#include "fep_system/base/properties/property_type.h"
#include "fep_system/base/properties/property_type_conversion.h"
#include <fep3/components/configuration/propertynode_helper.h>


template<typename T>
void testGetter(fep3::IConfigurationService& config_service,
                fep3::rpc::IRPCConfiguration& rpc_config,
                T value,
                const std::string& propertyname,
                const std::string& deeper_path)
{
    std::string path_with_slashes = deeper_path + "/" + propertyname;

    ASSERT_TRUE(fep3::isOk(fep3::setPropertyValue<T>(config_service, path_with_slashes, value)));

    auto properties_to_test = rpc_config.getProperties("/" + deeper_path);
    auto property_names = properties_to_test->getPropertyNames();
    ASSERT_TRUE(std::find(property_names.begin(), property_names.end(), propertyname) != property_names.end());

    ASSERT_EQ(properties_to_test->getPropertyType(propertyname), fep3::PropertyType<T>::getTypeName());

    ASSERT_EQ(fep3::DefaultPropertyTypeConversion<T>::fromString(properties_to_test->getProperty(propertyname)),
        value);
}


template<typename T>
void testSetter(fep3::IConfigurationService& config_service,
    fep3::rpc::IRPCConfiguration& rpc_config,
    T value,
    T init_value,
    std::string propertyname,
    const std::string& deeper_path)
{
    std::string propertypath_with_slashes = deeper_path + "/" + propertyname;

    ASSERT_TRUE(fep3::isOk(fep3::setPropertyValue<T>(config_service, propertypath_with_slashes, init_value)));

    auto properties_to_test = rpc_config.getProperties("/" + deeper_path);
    auto property_names = properties_to_test->getPropertyNames();
    ASSERT_TRUE(std::find(property_names.begin(), property_names.end(), propertyname) != property_names.end());

    ASSERT_EQ(properties_to_test->getPropertyType(propertyname), fep3::PropertyType<T>::getTypeName());

    ASSERT_TRUE(properties_to_test->setProperty(propertyname,
        fep3::DefaultPropertyTypeConversion<T>::toString(value),
        fep3::PropertyType<T>::getTypeName()));

    auto ret_value = fep3::getPropertyValue<T>(config_service, propertypath_with_slashes);
    ASSERT_TRUE(ret_value);
    ASSERT_EQ(*ret_value, value);
}


template<typename T>
inline void testSetGetInvalidFormat(fep3::rpc::IRPCConfiguration& rpc_config,
    T value,
    std::string property_path)
{
    auto properties_to_test = rpc_config.getProperties("/");
    auto property_names = properties_to_test->getPropertyNames();

    ASSERT_FALSE(properties_to_test->setProperty(property_path,
        fep3::DefaultPropertyTypeConversion<T>::toString(value),
        fep3::PropertyType<std::string>::getTypeName()));

    ASSERT_EQ("", properties_to_test->getProperty(property_path));

    ASSERT_EQ("", properties_to_test->getPropertyType(property_path));
}

template<typename T>
void testSetGetRoot(fep3::rpc::IRPCConfiguration& rpc_config,
    T initial_value,
    T value,
    std::string property_path,
    std::string property_name)
{
    std::string property_name_with_preceding_slash = "/" + property_name;
    auto properties_to_test = rpc_config.getProperties(property_path);

    ASSERT_EQ(properties_to_test->getProperty(property_name_with_preceding_slash),
        fep3::DefaultPropertyTypeConversion<T>::toString(initial_value));

    ASSERT_EQ(properties_to_test->getPropertyType(property_name_with_preceding_slash),
        fep3::PropertyType<T>::getTypeName());

    ASSERT_TRUE(properties_to_test->setProperty(property_name_with_preceding_slash,
        fep3::DefaultPropertyTypeConversion<T>::toString(value),
        fep3::PropertyType<T>::getTypeName()));

    ASSERT_EQ(properties_to_test->getProperty(property_name_with_preceding_slash),
        fep3::DefaultPropertyTypeConversion<T>::toString(value));
}


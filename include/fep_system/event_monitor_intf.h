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

#pragma once
#include <string>
#include "base/logging/logging_types.h"
#include "logging_types_legacy.h"

namespace fep3
{
/// Virtual class to implement FEP System Event Monitor, used with 
/// @ref fep3::System::registerMonitoring 
class IEventMonitor
{
public:
    /// DTOR
    virtual ~IEventMonitor() = default;

    /**
     * @brief Callback on log
     *
     * You will only retrieve log messages above the given fep::System::setSystemSeverityLevel
     *
     * @param log_time time of the log
     * @param severity_level severity level of the log
     * @param participant_name participant name (is empty on system category )
     * @param logger_name (usually the system, participant, component or element name and category)
     * @param message detailed message
     */
    virtual void onLog(std::chrono::milliseconds log_time,
        logging::Severity severity_level,
        const std::string& participant_name,
        const std::string& logger_name, //depends on the Category ... 
        const std::string& message) = 0;
};


/**
 * this is a namespace for porting convenience
 * this will be removed in further releases
 */
namespace legacy
{
/// Virtual class to implement FEP System Event Monitor, used with 
/// @ref fep3::System::registerMonitoring 
class EventMonitor : public IEventMonitor
{
public:
    /**
     * legacy callback by state change. has no effect and ist not supported anymore.
     *
     */
    virtual void onStateChanged(const std::string&,
        fep3::rpc::IRPCParticipantStateMachine::State)
    {
        //this is not supported in fep3 beta
        return;
    }

    /**
     * legacy callback by name change. has no effect and ist not supported anymore.
     *
     */
    virtual void onNameChanged(const std::string&, const std::string&)
    {
        //this is not supported anymore in fep3 
        return;
    }
    /**
     * @brief legacy callback on log at system, participant, component or element category.
     * This function will be removed in further releases. category was removed.
     *
     * You will only retrieve log messages above the given fep::System::setSystemSeverityLevel
     *
     * @param log_time time of the log
     * @param category category of the log
     * @param severity_level severity level of the log
     * @param participant_name participant name (is empty on system category )
     * @param logger_name (usually the system, participant, component or element name)
     * @param message detailed message
     */
    virtual void onLog(std::chrono::milliseconds log_time,
        logging::Category category,
        logging::Severity severity_level,
        const std::string& participant_name,
        const std::string& logger_name, //depends on the Category ... 
        const std::string& message) = 0;

    /**
     * @copydoc fep3::IEventMonitor::onLog
     */
    void onLog(std::chrono::milliseconds log_time,
        logging::Severity severity_level,
        const std::string& participant_name,
        const std::string& logger_name, //depends on the Category ... 
        const std::string& message) override
    {
        logging::legacy::Category category = logging::legacy::Category::CATEGORY_NONE;
        if (participant_name.empty())
        {
            category = logging::legacy::Category::CATEGORY_SYSTEM;
        }
        else
        {
            if (logger_name.find(".component") != std::string::npos)
            {
                category = logging::legacy::Category::CATEGORY_COMPONENT;
            }
            else if (logger_name.find(".element") != std::string::npos)
            {
                category = logging::legacy::Category::CATEGORY_ELEMENT;
            }
            else
            {
                category = logging::legacy::Category::CATEGORY_PARTICIPANT;
            }
        }
        onLog(log_time, category, severity_level, participant_name, logger_name, message);
    }
};
}
}

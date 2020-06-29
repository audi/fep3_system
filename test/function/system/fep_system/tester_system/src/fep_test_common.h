/**
 * Implementation of common auxiliary classes used by most of the FEP functional
 * test cases!
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

#ifndef _FEP_TEST_COMMON_H_INC_
#define _FEP_TEST_COMMON_H_INC_

#include <cmath>
#include <assert.h>

#include <a_util/system.h>
#include <a_util/logging.h>
#include <a_util/process.h>
#include <a_util/strings.h>
#include <thread>
#include <fep3/core.h>
#include <fep3/core/participant_executor.hpp>

#include <fep3/components/logging/logging_service_intf.h>


#define FEP_PREP_CMD_VERSION 1.0
#define FEP_PREP_REMOTE_PROP_TIMEOUT static_cast<timestamp_t>(5e6)

#ifdef WIN32
    #pragma warning( push )
    #pragma warning( disable : 4250 )
#endif

// This array contains all control events, i.e. all events that can be triggered
// remotely by ControlCommand.
/**
 * Create a platform (tester)-dependant name for stand-alone use.
 * @param [in] strOrigName  The original Module name
 * @return The modified name.
 */
static const std::string makePlatformDepName(const char* strOrigName)
{
    std::string strModuleNameDep(strOrigName);

    //strModuleNameDep.append(cTime::GetCurrentSystemTime().format("_%H%M%S"));

    #if (_MSC_VER == 1600)
        strModuleNameDep.append("_win64_vc100");
    #elif (_MSC_VER == 1900)
        strModuleNameDep.append("_win64_vc140");
    #elif (_MSC_VER >= 1910 && _MSC_VER < 1920)
        strModuleNameDep.append("_win64_vc141");
    #elif (defined (__linux))
        strModuleNameDep.append("_linux");
    #else
        // this goes for vc90, vc120 or apple or arm or whatever.
        #error "Platform currently not supported";
    #endif // Version check

    std::stringstream ss;
    ss << std::this_thread::get_id();

    strModuleNameDep += "_" + a_util::strings::toString(a_util::process::getCurrentProcessId());
    strModuleNameDep += "_" + ss.str();
    return strModuleNameDep;
}

struct TestElement : public fep3::core::ElementBase
{
    TestElement() 
        : fep3::core::ElementBase(makePlatformDepName("Testelement"), "3.0")
    {
    }
    fep3::Result load() override
    {
        _logger = getComponents()->getComponent<fep3::ILoggingService>()->createLogger("Testelement.element");
        _logger->logDebug("loading");
        _logger->logInfo("loading");
        _logger->logWarning("loading");
        _logger->logError("loading");
        _logger->logFatal("loading");
        return {};
    }

    fep3::Result initialize() override
    {
        _logger->logDebug("initializing");
        _logger->logInfo("initializing");
        _logger->logWarning("initializing");
        _logger->logError("initializing");
        _logger->logFatal("initializing");
        return {};
    }

    fep3::Result run() override
    {
        _logger->logDebug("running");
        _logger->logInfo("running");
        _logger->logWarning("running");
        _logger->logError("running");
        _logger->logFatal("running");
        return {};
    }

    std::shared_ptr<fep3::ILoggingService::ILogger> _logger;
    static constexpr const char* const _logger_name = "Testelement.element";
};

struct PartStruct
{
    PartStruct(PartStruct&&) = default;
    ~PartStruct() = default;
    PartStruct(fep3::core::Participant&& part) 
        : _part(std::move(part)), _part_executor(_part)
    {
    }
    fep3::core::Participant _part;
    fep3::core::ParticipantExecutor _part_executor;
};

using TestParticipants = std::map<std::string, std::unique_ptr<PartStruct>>;

/**
* @brief Creates modules from the incoming list of names
*
*/
inline TestParticipants createTestParticipants(
    const std::vector<std::string>& participant_names,
    const std::string& system_name)
{
    using namespace fep3::core;
    TestParticipants test_parts;
    std::for_each
        (participant_names.begin()
        , participant_names.end()
        , [&](const std::string& name)
            {
                auto part = createParticipant<ElementFactory<TestElement>>(name, "1.0", system_name);
                auto part_exec = std::make_unique<PartStruct>(std::move(part));
                part_exec->_part_executor.exec();
                test_parts[name].reset(part_exec.release());
            }
        );

    return std::move(test_parts);
}



#ifdef WIN32
    #pragma warning( pop )
#endif

#endif // _FEP_TEST_COMMON_H_INC_

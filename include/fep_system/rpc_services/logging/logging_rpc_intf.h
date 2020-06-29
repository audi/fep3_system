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

#include "logging_service_rpc_intf_def.h"
#include "../../base/logging/logging_types.h"
#include <string>

namespace fep3
{
namespace rpc
{
namespace arya
{
/**
* @brief definition of the external service interface for the logging service
* You will be register logging listeners and set configurations.
* @see logging_service.json file
*/
class IRPCLoggingService : public IRPCLoggingServiceDef
{
protected:
    /**
     * @brief Destroy the IRPCLogging object
     */
    virtual ~IRPCLoggingService() = default;

public:
    /**
     * @brief Sets the configuration for a given logger (domain)
     *
     * @param [in] logger_name       The logger name / domain to be configured
     * @param [in] configuration     A POD object that holds the configuration settings
     *
     * @return returns if the configuration is successful. Error details get logged.
     */
    virtual bool setLoggerFilter(const std::string& logger_name,
                                 const logging::LoggerFilter& configuration) const = 0;

    /**
     * @brief gets the configuration for a given logger (domain)
     *
     * @param [in] logger_name       The logger name / domain to get the current set filter for
     *
     * @return returns the configured loggerfilter if found. otherwise it is empty.
     */
    virtual logging::LoggerFilter getLoggerFilter(const std::string& logger_name) const = 0;

    /**
     * @brief gets a list of the available loggers
     *
     * @return returns a list of the available loggers
     */
    virtual std::vector<std::string> getLoggers() const = 0;

    /**
     * @brief gets a list of the current available sinks
     *
     * @return returns a list of the current available sinks
     */
    virtual std::vector<std::string> getSinks() const = 0;

    /**
     * @brief get the properties of the given sink.
     *
     * @param [in] sink_name name of the sink 
     *
     * @return returns the properties of the sink
     */
    virtual std::shared_ptr<IProperties> getProperties(const std::string& sink_name) const = 0;
};

/**
 * The RPC Logging Sink Service interface provide functionality to register and unregister 
 * a server address where the log messages can be send to.
 * 
 */
class IRPCLoggingSinkService : public IRPCLoggingSinkServiceDef
{
protected:
    /**
     * @brief Destroy the Logging Sink Service object
     */
    virtual ~IRPCLoggingSinkService() = default;
public:
    /**
    * @brief registers the address at the service to receive messages
    *
    * @param [in] url valid url to send the log messages to
    *
    * @return returns 0 if succeeded
    */
    virtual int registerRPCClient(const std::string& url) = 0;
    /**
    * @brief unregisters the address from the service to stop sending log messages to that address
    *
    * @param [in] url valid url where the messages was sent to
    *
    * @return returns 0 if succeeded
    */
    virtual int unregisterRPCClient(const std::string& url) = 0;
};
} // namespace arya
using arya::IRPCLoggingSinkService;
using arya::IRPCLoggingService;
} // namespace rpc
} // namespace fep3
/**
* @file
*
* @copyright
* @verbatim
Copyright @ 2020 AUDI AG. All rights reserved.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

If it is not possible or desirable to put the notice in a particular file, then
You may include the notice in a location (such as a LICENSE file in a
relevant directory) where a recipient would be likely to look for such a notice.

You may add additional accurate notices of copyright ownership.
@endverbatim
*/

#pragma once

#include "base/logging/logging_types.h"
#include <string>
#include <chrono>

namespace fep3
{
    /**
     * @brief The system logger is an internal interfaces used within the ParticipantProxy
     * It is the connection to the IEventMonitor set within the fep3::System class.
     * Each logging message within the ParticipantProxy which is part of a fep3::System will be forwarded to this
     * interface.
     * 
     */
    class ISystemLogger
    {
        protected:
            /**
             * @brief default DTOR
             * 
             */
            virtual ~ISystemLogger() = default;
        public:
            /**
             * @brief logs a log message to the system logger.
             * 
             * @param timestamp time value
             * @param level Level 
             * @param participant_name The name of the participant 
             * @param logger_name the name of the logger
             * @param message the message that will be forwarded
             */
            virtual void log(const std::chrono::milliseconds& timestamp,
                             logging::Severity level,
                             const std::string& participant_name,
                             const std::string& logger_name, //depends on the Category ... 
                             const std::string& message) const = 0;
            /**
             * @brief logs a log message to the system logger.
             * time value is taken from now
             *
             * @param level Level
             * @param participant_name The name of the participant
             * @param logger_name the name of the logger
             * @param message the message that will be forwarded
             */
            virtual void log(
                logging::Severity level,
                const std::string& participant_name,
                const std::string& logger_name, //depends on the Category ... 
                const std::string& message) const = 0;

            /**
             * @brief gets a valid url for registering the system instance to the participants logging service.
             *
             * @return returns a valid url for registering the system instance to the participants logging service.
             */
            virtual std::string getUrl() const = 0;
    };
}

/**
 * @brief Helper macro to log a message to a system logger
 * @param given_logger logger pointer 
 * @param given_sev Level 
 * @param given_part_name The name of the participant 
 * @param given_logger_name the name of the logger
 * @param log_message the message that will be forwarded
 */
#define FEP3_SYSTEM_LOG(given_logger, given_sev, given_part_name, given_logger_name, log_message) \
do \
{ \
    given_logger->log(\
        given_sev, \
        given_part_name, \
        given_logger_name, \
        std::string() + log_message); \
} while (false)

/**
 * @brief Helper macro to log to a system logger a mesage and throw a runtime_error
 * @param given_logger logger pointer 
 * @param given_sev Level 
 * @param given_part_name The name of the participant 
 * @param given_logger_name the name of the logger
 * @param log_message the message that will be forwarded
 */
#define FEP3_SYSTEM_LOG_AND_THROW(given_logger, given_sev, given_part_name, given_logger_name, log_message) \
do \
{ \
    std::string log_message_string = std::string() + log_message; \
    given_logger->log(\
        given_sev, \
        given_part_name, \
        given_logger_name, \
        log_message_string); \
        throw std::runtime_error(log_message_string); \
} while (false)

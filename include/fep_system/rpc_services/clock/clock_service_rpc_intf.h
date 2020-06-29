/**
 * Declaration of the Class IRPCParticipantInfo. (can be reached from over rpc)
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

#pragma once

#include <vector>
#include <string>

#include "clock_service_rpc_intf_def.h"


namespace fep3
{
namespace rpc
{
namespace arya
{

/**
* @brief definition of the external clock service interface of the participant
* @see clock.json file
*/
class IRPCClockService : public IRPCClockServiceDef
{
protected:
    /**
     * @brief Destroy the IRPCClockService object
     *
     */
    virtual ~IRPCClockService() = default;
public:
    /**
     * Type of the clock.
     */
    enum ClockType : int32_t
    {
        ///invalid type
        invalid = -1,
        ///continues clock (or steady)
        continuous_clock = 0,
        ///discrete clock with discrete steps
        discrete_clock = 1
    };
    /**
     * retrieves the list of the names of all registered clocks of the corresponding participant
     * @return comma seperated list of the names of all registered clocks
     */
    virtual std::vector<std::string> getClockNames() const = 0;

    /**
     * retrieves the names of the current main clock configured out of the registered clocknames
     * @return the name of all main clock
     */
    virtual std::string getMainClockName() const = 0;

    /**
     * get time in nano sec for the given clock
     * if clock_name is empty it will return the time of the current getMainClockName
     * 
     * @param clock_name name of the clock to retrieve the current time from
     * @return value of the time in nanosec resolution
     * @retval -1 if the clock does not exist
     */
    virtual int64_t getTime(const std::string& clock_name) const = 0;

    /**
     * get the type in for the given clock
     * if clock_name is empty it will return the type of the current getMainClockName
     *
     * @param clock_name name of the clock to retrieve the current type from
     * @return value of the type
     * @retval ClockType::invalid if the clock does not exist
     */
    virtual ClockType getType(const std::string& clock_name) const = 0;
};
}
using arya::IRPCClockService;
}
}

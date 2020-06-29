/**
 * @file
 * Declaration of the Class IRPCParticipantInfo. (can be reached from over rpc)

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
#include "participant_info_rpc_intf_def.h"

namespace fep3
{
namespace rpc
{
namespace arya
{
/**
* @brief definition of the external service interface of the participant itself
* @see participant_info.json file
*/
class IRPCParticipantInfo : public IRPCParticipantInfoDef
{
protected:
    /**
     * @brief Destroy the IRPCParticipantInfo object
     *
     */
    virtual ~IRPCParticipantInfo() = default;

public:
    /**
     * @brief Get the Name
     *
     * @return std::string the name
     * @throw runtime_error if timeout occurred
     */
    virtual std::string getName() const = 0;
    /**
     * @brief Get the System Name the participant belongs to
     *
     * @return std::string the system name
     * @throw runtime_error if timeout occurred
     * @remark this is not yet implemented
     */
    virtual std::string getSystemName() const = 0;

    /**
     * @brief get the names of all registered rpc component object within the participant
     *
     * @return std::vector<std::string> the list of the names
     * @throw runtime_error if timeout occurred
     */
    virtual std::vector<std::string> getRPCComponents() const = 0;
    /**
     * @brief get the identifier of all interfaces the rpc component object supports
     *
     * @param rpc_component_name the component name to retrieve the interface ids from
     * @return std::vector<std::string> the list of the rpc interface identifiers
     * @throw runtime_error if timeout occurred
     */
    virtual std::vector<std::string> getRPCComponentIIDs(const std::string& rpc_component_name) const = 0;
    /**
     * @brief gets the fully interface definition of the interface of the rpc component
     *
     * @param rpc_component_name the component name to get the interface definition from
     * @param rpc_component_iid the interface id to get the interface definition of
     * @return std::string the interface definiton as string
     * @throw runtime_error if timeout occurred
     */
    virtual std::string getRPCComponentInterfaceDefinition(const std::string& rpc_component_name,
        const std::string& rpc_component_iid) const = 0;
};
}
using arya::IRPCParticipantInfo;
}
}

/**
 * Declaration of the Class IRPCDataRegistry. (can be reached from over rpc)
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

#ifndef __FEP_RPC_DATA_REGISTRY_INTF_H
#define __FEP_RPC_DATA_REGISTRY_INTF_H

#include <vector>
#include <string>
#include "data_registry_rpc_intf_def.h"
#include "../../base/streamtype/streamtype.h"

namespace fep3
{
namespace rpc
{
namespace arya
{
/**
 * @brief definition of the external service interface of the data registry
 * @see data_registry.json file
 */
class IRPCDataRegistry : public IRPCDataRegistryDef
{
protected:
    /**
     * @brief Destroy the IRPCDataRegistry object
     *
     */
    virtual ~IRPCDataRegistry() = default;

public:
    /**
     * @brief Get a list of the signal names going in
     * @remark Since this information is retrieved with the FEP 2 Signal Registry
     *         and the FEP Developer may register its Signal while initializing state
     *         a reliable return value is only possible while state ready has been reached.
     *
     * @return std::vector<std::string> signal list names
     */
    virtual std::vector<std::string> getSignalsIn() const = 0;
    /**
     * @brief Get a list of the signal names going out
     * @remark Since this information is retrieved with the FEP 2 Signal Registry
     *         and the FEP Developer may register its Signal while initializing state
     *         a reliable return value is only possible while state ready has been reached.
     *
     * @return std::vector<std::string> signal list names
     */
    virtual std::vector<std::string> getSignalsOut() const = 0;

    /**
     * @brief Get the Stream Type for the given signal name
     *
     * @param signal_name the signal name
     * @return StreamType Usually FEP does only support fep3::StreamTypeRaw, fep3::StreamTypeDDL
     */
    virtual StreamType getStreamType(const std::string& signal_name) const = 0;
};
}
using arya::IRPCDataRegistry;
}
}

#endif // __FEP_RPC_PARTICIPANT_INFO_INTF_H

/**
 * Declaration of the Class IRPCConfiguration. (can be reached from over rpc)
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

#ifndef __FEP_RPC_CONFIGURATION_INTF_H
#define __FEP_RPC_CONFIGURATION_INTF_H

#include <vector>
#include <string>
#include <memory>
#include "configuration_rpc_intf_def.h"
#include "../../base/properties/properties_intf.h"


namespace fep3
{
namespace rpc
{
namespace arya
{
/**
 * @brief definition of the external service interface for the participants configuration
 * You will be able to set and get properties.
 * @see configuration.json file
 */
class IRPCConfiguration : public IRPCConfigurationDef
{
protected:
    /**
     * @brief Destroy the IRPCConfiguration object
     *
     */
    virtual ~IRPCConfiguration() = default;

public:
    /**
     * @brief retrieve the instance of one properties node within the tree.
     * to retrieve root use \p "/" or "" (empty) string.
     *
     * @param property_path path to the properties
     *
     *
     * @throw If the connection has a timeout it will throw a runtime error
     *
     * @return returns a connection to the IProperties node.
     */
    virtual std::shared_ptr<IProperties> getProperties(const std::string& property_path) = 0;
    /**
     * @brief retrieve the const instance of one properties node within the tree.
     * to retrieve root use \p "/" or "" (empty) string.
     *
     * @param property_path path to the properties
     *
     * If the connection has a timeout it will throw a runtime error.
     *
     *
     * @return returns a connection to the IProperties node.
     * @retval empty_shared_ptr property does not exists
     */
    virtual std::shared_ptr<const IProperties> getProperties(const std::string& property_path) const = 0;
};
}
using arya::IRPCConfiguration;
}
}

#endif // __FEP_RPC_CONFIGURATION_INTF_H

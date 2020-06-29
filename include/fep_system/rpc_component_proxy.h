/**
 *
 * RPC Component alias definition.
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
#include "rpc_services/base/fep_rpc_client.h" 

namespace fep3
{
namespace arya
{
/**
 * @brief alias definition for a clear API.
 * The IRPCComponentPtr represents the RPC Service Interface of one component.
 *
 */
using IRPCComponentPtr = fep3::rpc::arya::IRPCClientPtr;

/**
 * @brief alias definition for a clear API.
 * Helper smart pointer for reference counting of the component interfaces
 *
 * @tparam T The IRPCPbject
 */
template<typename T>
using RPCComponent = fep3::rpc::arya::RPCClient<T>;
}
using arya::IRPCComponentPtr;
using arya::RPCComponent;
}

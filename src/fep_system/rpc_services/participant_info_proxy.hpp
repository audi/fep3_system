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

#include <components/service_bus/rpc/fep_rpc_stubs_client.h>
#include <fep_system_stubs/participant_info_proxy_stub.h>

#include "rpc_services/participant_info/participant_info_rpc_intf.h"
#include <a_util/strings.h>

namespace fep3
{
namespace rpc
{
namespace arya
{

class ParticipantInfoProxy : public RPCServiceClientProxy <
    rpc_proxy_stub::RPCParticipantInfoProxy,
    IRPCParticipantInfo>
{
private:
    typedef RPCServiceClientProxy< rpc_proxy_stub::RPCParticipantInfoProxy, IRPCParticipantInfo> base_type;

public:
    using base_type::GetStub;
    ParticipantInfoProxy(std::string rpc_component_name,
        std::shared_ptr<rpc::IRPCRequester> rpc) :
        base_type(rpc_component_name, rpc)
    {
    }

    std::string getName() const override
    {
        try
        {
            return GetStub().getName();
        }
        catch (...)
        {
            return std::string();
        }
    }
    std::string getSystemName() const override
    {
        try
        {
            return GetStub().getSystemName();
        }
        catch (...)
        {
            return std::string();
        }
    }

    std::vector<std::string> getRPCComponents() const override
    {
        try
        {
            std::string returned_list = GetStub().getRPCServices();
            return a_util::strings::split(returned_list, ";");
        }
        catch (...)
        {
            return  std::vector<std::string>();
        }
    }
    std::vector<std::string> getRPCComponentIIDs(const std::string& rpc_component_name) const override
    {
        try
        {
            std::string returned_list = GetStub().getRPCServiceIIDs(rpc_component_name);
            return a_util::strings::split(returned_list, ";");
        }
        catch (...)
        {
            return  std::vector<std::string>();
        }
    }
    std::string getRPCComponentInterfaceDefinition(const std::string& rpc_component_name,
        const std::string& rpc_component_iid) const override
    {
        try
        {
            return GetStub().getRPCServiceInterfaceDefinition(rpc_component_name, rpc_component_iid);
        }
        catch (...)
        {
            return std::string();
        }
    }

};
}
}
}
